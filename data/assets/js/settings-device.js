document.addEventListener("DOMContentLoaded", function () {
	// Hide all alerts initially
	const hideAllAlerts = () => {
		document.getElementById("alert-saved-wled").style.display = "none";
		document.getElementById("alert-saved-analog").style.display = "none";
		document.getElementById("alert-saved-switch").style.display = "none";
	};

	// Function to save settings in local storage
	function saveSettings() {
		const settings = {
			wled: {
				activate: document.getElementById("wled-activate").checked,
				count: document.getElementById("wled-count").value,
				order: document.getElementById("wled-order").value,
			},
			analog: {
				activate: document.getElementById("analog-activate").checked,
				mode: document.getElementById("analog-mode").value
			},
			switch: {
				activate: document.getElementById("switch-activate").checked,
				function: document.getElementById("switch-function").value,
			},
		};

		// Save the settings as JSON string in local storage
		localStorage.setItem("deviceSettings", JSON.stringify(settings));
		console.log("Settings saved:", settings);
	}

	// Function to load settings from local storage
	function loadSettings() {
		const savedSettings = localStorage.getItem("deviceSettings");

		if (savedSettings) {
			const settings = JSON.parse(savedSettings);

			// Load WLED settings
			document.getElementById("wled-activate").checked = settings.wled.activate;
			document.getElementById("wled-count").value = settings.wled.count;
			document.getElementById("wled-order").value = settings.wled.order;

			// Load Analog settings
			document.getElementById("analog-activate").checked = settings.analog.activate;
			document.getElementById("analog-mode").value = settings.analog.mode;

			// Load Switch settings
			document.getElementById("switch-activate").checked = settings.switch.activate;
			document.getElementById("switch-function").value = settings.switch.function;

			// Now, ensure the correct sections are hidden or shown based on the settings
			toggleSection("wled-activate", ".wled-settings");
			toggleSection("analog-activate", ".analog-settings");
			toggleSection("switch-activate", ".switch-settings");

			console.log("Settings loaded:", settings);
		}
	}

	// Function to scroll to the top of the page
	function scrollToTop() {
		window.scrollTo({ top: 0, behavior: "smooth" });
	}

	// Show a specific alert for a few seconds
	const showAlert = (alertId) => {
		const alert = document.getElementById(alertId);
		alert.style.display = "block";

		setTimeout(() => {
			alert.style.display = "none";
		}, 3000);
		scrollToTop();
	};

	// Form handling for WLED
	document.getElementById("form-wled").addEventListener("submit", function (event) {
		event.preventDefault();
		// Gather form data
		const activate = document.getElementById("wled-activate").checked;
		const ledCount = document.getElementById("wled-count").value;
		const colorOrder = document.getElementById("wled-order").value;

		console.log("Saving WLED settings:", { activate, ledCount, colorOrder });

		// Show success alert
		showAlert("alert-saved-wled");
		saveSettings();
	});

	// Form handling for Analog LED
	document.getElementById("form-analog").addEventListener("submit", function (event) {
		event.preventDefault();
		const activate = document.getElementById("analog-activate").checked;
		const mode = document.getElementById("analog-mode").value;

		console.log("Saving Analog settings:", { activate, mode });

		// Show success alert
		showAlert("alert-saved-analog");
		saveSettings();
	});

	// Form handling for Switch settings
	document.getElementById("form-switch").addEventListener("submit", function (event) {
		event.preventDefault();
		const activate = document.getElementById("switch-activate").checked;
		const switchFunction = document.getElementById("switch-function").value;

		console.log("Saving Switch settings:", { activate, switchFunction });

		// Show success alert
		showAlert("alert-saved-switch");
		saveSettings();
	});

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

	// Toggle visibility of settings based on activation checkboxes
	toggleSection("wled-activate", ".wled-settings");
	toggleSection("analog-activate", ".analog-settings");
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
		const modal = bootstrap.Modal.getInstance(document.getElementById("modal-reset"));
		modal.hide();
	});

	modalCloseButton.addEventListener("click", function () {
		const modal = bootstrap.Modal.getInstance(document.getElementById("modal-reset"));
		modal.hide();
	});

	// Initially hide all alerts
	hideAllAlerts();
	loadSettings();
});
