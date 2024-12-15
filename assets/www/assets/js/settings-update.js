async function uploadOTA() {
	showOtaProgress();

	const file = document.getElementById("ota-selector").files[0];
	const formData = new FormData();
	formData.append("file", file);

	try {
		const response = await fetch("/api/ota-upload", {
			method: "POST",
			body: formData,
		});

		if (response.ok) {
			console.log(response.text());
		}
	} catch (error) {
		console.error("Error uploading OTA file: ", error);
	}
}

async function showOtaProgress() {
    const modal = new bootstrap.Modal(document.getElementById("modal-ota"));
	document.getElementById("ota-progress-txt").innerText = "Installing...";
	document.querySelector(".progress-bar").style.width = "0%";
	document.querySelector(".progress-bar").innerText = "0%";
	document.querySelector(".progress-bar").classList.remove("bg-danger");
	document.querySelector(".progress-bar").classList.add("bg-success");
	document.querySelector(".progress-bar").classList.add("progress-bar-striped");
	document.querySelector(".progress-bar").classList.add("progress-bar-animated");
	modal.show();

	if (!!window.EventSource) {
		var source = new EventSource("/events");

		source.addEventListener('open', function(e) {
			console.log("Events Connected");
		}, false);

		source.addEventListener("ota-progress", function (e) {
				console.log("OTA Progress: " + e.data + "%");
				document.getElementById("ota-progress-txt").innerText = "Installing: " + e.data + "%";

				document.querySelector(".progress-bar").style.width = e.data + "%";
				document.querySelector(".progress-bar").innerText = e.data + "%";

				if (e.data == 100) {
					document.querySelector(".progress-bar").classList.remove("progress-bar-striped");
					document.querySelector(".progress-bar").classList.remove("progress-bar-animated");
					modal.hide();

					// show success modal
					const modalSuccess = new bootstrap.Modal(document.getElementById("modal-ota-complete"));
					modalSuccess.show();

					setTimeout(() => {
						window.location.reload();
					}, 5000);
				}

		}, false);

		source.addEventListener("error", function (e) {
			if (e.readyState == EventSource.CLOSED) {
				console.error("Connection closed");
				document.getElementById("ota-progress-txt").innerText = "Connection closed";

				document.querySelector(".progress-bar").style.width = "100%";
				document.querySelector(".progress-bar").innerText = "100%";
				document.querySelector(".progress-bar").classList.remove("bg-success");
				document.querySelector(".progress-bar").classList.add("bg-danger");
				document.querySelector(".progress-bar").classList.remove("progress-bar-striped");
				document.querySelector(".progress-bar").classList.remove("progress-bar-animated");

				modal.hide();

				// show fail modal
				const modalFail = new bootstrap.Modal(document.getElementById("modal-ota-failed"));
				modalFail.show();

				setTimeout(() => {
					window.location.reload();
				}, 5000);
			}
		}, false);
	}
}

document.getElementById("ota-upload").addEventListener("click", function (event) {
	event.preventDefault();
	uploadOTA();
});
