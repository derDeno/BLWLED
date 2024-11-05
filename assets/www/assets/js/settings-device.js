async function loadDeviceSettings() {
	try {
		const response = await fetch("/api/settings-device");
		if (response.ok) {
			const settings = await response.json();
			document.getElementById("wled-activate").checked = settings.wled;
			document.getElementById("wled-count").value = settings.count || '';
			document.getElementById("wled-order").value = settings.order || '';
			document.getElementById("analog-activate").checked = settings.analog;
			document.getElementById("analog-mode").value = settings.mode || '';
			document.getElementById("switch-activate").checked = settings.switch;
			document.getElementById("switch-action").value = settings.action || '';
			document.getElementById("debug-log-activate").checked = settings.logging;

			// Toggle visibility of settings based on activation checkboxes
			toggleSection("wled-activate", ".wled-settings");
			toggleSection("analog-activate", ".analog-settings");
			toggleSection("switch-activate", ".switch-settings");
		} else {
			console.error("Failed to load device settings");
		}
	} catch (error) {
		console.error("Error fetching device settings:", error);
	}
}

async function saveDeviceSettings() {
	const wledActivate = document.getElementById("wled-activate").checked;
	const wledCount = document.getElementById("wled-count").value;
	const wledOrder = document.getElementById("wled-order").value;
	const analogActivate = document.getElementById("analog-activate").checked;
	const analogMode = document.getElementById("analog-mode").value;
	const switchActivate = document.getElementById("switch-activate").checked;
	const switchAction = document.getElementById("switch-action").value;
	const loggingActivate = document.getElementById("debug-log-activate").checked;

	const formData = new URLSearchParams();
	formData.append("wled", wledActivate);
	formData.append("count", wledCount);
	formData.append("order", wledOrder);
	formData.append("analog", analogActivate);
	formData.append("mode", analogMode);
	formData.append("switch", switchActivate);
	formData.append("action", switchAction);
	formData.append("logging", loggingActivate);

	try {
		const response = await fetch("/api/settings-device", {
			method: "POST",
			headers: {
				"Content-Type": "application/x-www-form-urlencoded",
			},
			body: formData.toString(),
		});

		if (response.ok) {
			const alertBox = document.getElementById("alert-saved-device");
			alertBox.style.display = "block";

			setTimeout(() => {
				alertBox.style.display = "none";
			}, 3000);
		} else {
			console.error("Failed to save device settings");
		}
	} catch (error) {
		console.error("Error saving device settings:", error);
	}
}

// Function to scroll to the top of the page
function scrollToTop() {
	window.scrollTo({ top: 0, behavior: "smooth" });
}

// Function to hide or show settings inputs based on checkbox status
function toggleSection(checkboxId, sectionClass) {
	const checkbox = document.getElementById(checkboxId);
	const section = document.querySelector(sectionClass);

	if (checkbox.checked) {
		section.style.display = "block";
	} else {
		section.style.display = "none";
	}

	checkbox.addEventListener("change", function () {
		if (this.checked) {
			section.style.display = "block";
		} else {
			section.style.display = "none";
		}
	});
}

document.getElementById("btn-save-device").addEventListener("click", function (e) {
	e.preventDefault();
	saveDeviceSettings();
});

document.getElementById("btn-reset").addEventListener("click", function () {
	const modal = new bootstrap.Modal(document.getElementById("modal-reset"));
	modal.show();
});

document.getElementById("btn-reset-modal").addEventListener("click", async function () {
	console.log("Device reset confirmed");
	// Add logic to reset device settings here

	try {
		const response = await fetch("/api/reset", {
			method: "POST",
		});

		if (response.ok) {

			const modal = bootstrap.Modal.getInstance(document.getElementById("modal-reset"));
			modal.hide();
			
		} else {
			console.error("Failed to save device settings");
		}
	} catch (error) {
		console.error("Error saving device settings:", error);
	}
});

document.getElementById("btn-modal-close").addEventListener("click", function () {
	const modal = bootstrap.Modal.getInstance(document.getElementById("modal-reset"));
	modal.hide();
});


window.onload = function () {
    loadDeviceSettings();
    document.getElementById('alert-saved-device').style.display = 'none';
};