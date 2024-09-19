// Wait until the DOM is fully loaded
document.addEventListener("DOMContentLoaded", function () {
	// Hide all alerts initially
	const hideAllAlerts = () => {
		document.getElementById("alert-saved-wled").style.display = "none";
		document.getElementById("alert-saved-analog").style.display = "none";
		document.getElementById("alert-saved-switch").style.display = "none";
	};

	// Show a specific alert for a few seconds
	const showAlert = (alertId) => {
		const alert = document.getElementById(alertId);
		alert.style.display = "block";
		setTimeout(() => {
			alert.style.display = "none";
		}, 3000);
	};

	// Form handling for WLED
	document
		.getElementById("form-wled")
		.addEventListener("submit", function (event) {
			event.preventDefault();
			// Gather form data
			const activate = document.getElementById("wled-activate").checked;
			const ledCount = document.getElementById("wled-count").value;
			const colorOrder = document.getElementById("wled-order").value;

			console.log("Saving WLED settings:", { activate, ledCount, colorOrder });

			// Show success alert
			showAlert("alert-saved-wled");
		});

	// Form handling for Analog LED
	document
		.getElementById("form-analog")
		.addEventListener("submit", function (event) {
			event.preventDefault();
			const activate = document.getElementById("analog-activate").checked;
			const mode = document.getElementById("analog-mode").value;

			console.log("Saving Analog settings:", { activate, mode });

			// Show success alert
			showAlert("alert-saved-analog");
		});

	// Form handling for Switch settings
	document
		.getElementById("form-switch")
		.addEventListener("submit", function (event) {
			event.preventDefault();
			const activate = document.getElementById("switch-activate").checked;
			const switchFunction = document.getElementById("switch-function").value;

			console.log("Saving Switch settings:", { activate, switchFunction });

			// Show success alert
			showAlert("alert-saved-switch");
		});

	// Handle Analog mode toggle between "LED Strip" and "Signal Tower"
	document
		.getElementById("analog-mode")
		.addEventListener("change", function () {
			const mode = this.value;
			const ledstripFieldset = document.getElementById("fieldset-ledstrip");
			const towerFieldset = document.getElementById("fieldset-signaltower");

			if (mode === "strip") {
				ledstripFieldset.style.display = "block";
				towerFieldset.style.display = "none";
			} else {
				ledstripFieldset.style.display = "none";
				towerFieldset.style.display = "block";
			}
		});

	// Initialize the Analog mode fieldset visibility
	const initialMode = document.getElementById("analog-mode").value;
	if (initialMode === "strip") {
		document.getElementById("fieldset-ledstrip").style.display = "block";
		document.getElementById("fieldset-signaltower").style.display = "none";
	} else {
		document.getElementById("fieldset-ledstrip").style.display = "none";
		document.getElementById("fieldset-signaltower").style.display = "block";
	}

	// Function to hide or show settings inputs based on checkbox status
	function toggleSection(checkboxId, sectionClass) {
		const checkbox = document.getElementById(checkboxId);
		const section = document.querySelector(sectionClass);

		// This will initially show or hide the section depending on the checkbox state
		if (checkbox.checked) {
			section.style.display = "block";
		} else {
			section.style.display = "none";
		}

		// Add event listener for checkbox toggle
		checkbox.addEventListener("change", function () {
			if (this.checked) {
				section.style.display = "block";
			} else {
				section.style.display = "none";
			}
		});
	}

	// Toggle visibility of WLED settings fields (without hiding the checkbox)
	toggleSection("wled-activate", ".wled-settings");

	// Toggle visibility of Analog LED settings fields (without hiding the checkbox)
	toggleSection("analog-activate", ".analog-settings");

	// Toggle visibility of Switch settings fields (without hiding the checkbox)
	toggleSection("switch-activate", ".switch-settings");

	// Modal handling for reset
	const resetButton = document.getElementById("btn-reset");
	const modalResetButton = document.getElementById("btn-reset-modal");
	const modalCloseButton = document.getElementById("btn-modal-close");

	resetButton.addEventListener("click", function () {
		const modal = new bootstrap.Modal(document.getElementById("modal-reset"));
		modal.show();
	});

	modalResetButton.addEventListener("click", function () {
		console.log("Device reset confirmed");
		// Add logic to reset device settings here
		const modal = bootstrap.Modal.getInstance(
			document.getElementById("modal-reset")
		);
		modal.hide();
	});

	modalCloseButton.addEventListener("click", function () {
		const modal = bootstrap.Modal.getInstance(
			document.getElementById("modal-reset")
		);
		modal.hide();
	});

	// Initially hide all alerts
	hideAllAlerts();
});
