async function uploadOTA() {
	const formData = new FormData();
	const file = document.getElementById("ota-selector").files[0];
	formData.append("file", file);

	try {
		const response = await fetch("/api/ota-upload", {
			method: "POST",
			headers: {
				"Content-Type": "multipart/form-data",
			},
			body: formData,
		});

		if (response.ok) {
			console.log("OTA file uploaded successfully");
            showOtaProgress();
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
	document.getElementById("modal-ota-close").style.display = "none";
	modal.show();

	if (!!window.EventSource) {
		var source = new EventSource("/events");

		source.addEventListener(
			"ota-progress",
			function (e) {
				console.log("OTA Progress: " + e.data + "%");
				document.getElementById("ota-progress-txt").innerText = "Installing: " + e.data + "%";

				document.querySelector(".progress-bar").style.width = e.data + "%";
				document.querySelector(".progress-bar").innerText = e.data + "%";
			},
			false
		);

		source.addEventListener(
			"error",
			function (e) {
				if (e.readyState == EventSource.CLOSED) {
					console.error("Connection closed");
					document.getElementById("ota-progress-txt").innerText = "Connection closed";

					document.querySelector(".progress-bar").style.width = "100%";
					document.querySelector(".progress-bar").innerText = "100%";
					document.querySelector(".progress-bar").classList.remove("bg-success");
					document.querySelector(".progress-bar").classList.add("bg-danger");
					document.querySelector(".progress-bar").classList.remove("progress-bar-striped");
					document.querySelector(".progress-bar").classList.remove("progress-bar-animated");
					document.getElementById("modal-ota-close").style.display = "block";
				}
			},
			false
		);
	}
}

document.getElementById("ota-upload").addEventListener("click", function (event) {
	event.preventDefault();
	uploadOTA();
});

document.getElementById("modal-ota-close").addEventListener("click", function (event) {
	event.preventDefault();
	const modal = new bootstrap.Modal(document.getElementById("modal-ota"));
	modal.hide();
});
