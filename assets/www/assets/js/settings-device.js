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
				mode: document.getElementById("analog-mode").value,
				led: {
					r: document.getElementById("analog-led-r").value,
					g: document.getElementById("analog-led-g").value,
					b: document.getElementById("analog-led-b").value,
					ww: document.getElementById("analog-led-ww").value,
					cw: document.getElementById("analog-led-cw").value,
				},
				tower: {
					r: document.getElementById("analog-tower-r").value,
					g: document.getElementById("analog-tower-g").value,
					b: document.getElementById("analog-tower-b").value,
					ww: document.getElementById("analog-tower-ww").value,
					cw: document.getElementById("analog-tower-cw").value,
				},
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

			// Load LED Strip settings
			document.getElementById("analog-led-r").value = settings.analog.led.r;
			document.getElementById("analog-led-g").value = settings.analog.led.g;
			document.getElementById("analog-led-b").value = settings.analog.led.b;
			document.getElementById("analog-led-ww").value = settings.analog.led.ww;
			document.getElementById("analog-led-cw").value = settings.analog.led.cw;

			// Load Signal Tower settings
			document.getElementById("analog-tower-r").value = settings.analog.tower.r;
			document.getElementById("analog-tower-g").value = settings.analog.tower.g;
			document.getElementById("analog-tower-b").value = settings.analog.tower.b;
			document.getElementById("analog-tower-ww").value = settings.analog.tower.ww;
			document.getElementById("analog-tower-cw").value = settings.analog.tower.cw;

			// Load Switch settings
			document.getElementById("switch-activate").checked = settings.switch.activate;
			document.getElementById("switch-function").value = settings.switch.function;

			// Now, ensure the correct sections are hidden or shown based on the settings
			toggleSection("wled-activate", ".wled-settings");
			toggleSection("analog-activate", ".analog-settings");
			toggleSection("switch-activate", ".switch-settings");

			// Show the correct analog mode (LED Strip or Signal Tower)
			updateAnalogMode(settings.analog.mode);

			console.log("Settings loaded:", settings);
		}
	}

	// Function to update the analog mode fieldset visibility
	function updateAnalogMode(mode) {
		const ledstripFieldset = document.getElementById("fieldset-ledstrip");
		const towerFieldset = document.getElementById("fieldset-signaltower");

		if (mode === "strip") {
			ledstripFieldset.style.display = "block";
			towerFieldset.style.display = "none";
		} else {
			ledstripFieldset.style.display = "none";
			towerFieldset.style.display = "block";
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

	// Handle Analog mode toggle between "LED Strip" and "Signal Tower"
	document.getElementById("analog-mode").addEventListener("change", function () {
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

	// Function to update the dropdowns based on selected values
	function updateAnalogPinOptions() {
		// Get the values of all pin dropdowns
		const rPin = document.getElementById("analog-led-r");
		const gPin = document.getElementById("analog-led-g");
		const bPin = document.getElementById("analog-led-b");
		const wwPin = document.getElementById("analog-led-ww");
		const cwPin = document.getElementById("analog-led-cw");

		const allPins = [rPin, gPin, bPin, wwPin, cwPin];

		// Gather selected values (ignore "inactive")
		const selectedValues = {
			r: rPin.value !== "inactive" ? rPin.value : null,
			g: gPin.value !== "inactive" ? gPin.value : null,
			b: bPin.value !== "inactive" ? bPin.value : null,
			ww: wwPin.value !== "inactive" ? wwPin.value : null,
			cw: cwPin.value !== "inactive" ? cwPin.value : null,
		};

		// Reset all options to be enabled and reset label modification
		allPins.forEach((pin) => {
			Array.from(pin.options).forEach((option) => {
				// Always keep "inactive" enabled
				if (option.value !== "inactive") {
					option.disabled = false;
					// Remove any label modification
					option.textContent = option.textContent.replace(/ - .*/, ""); // remove any existing " - Pin" text
				}
			});
		});

		// Disable selected options in other dropdowns and append the pin name to the label
		Object.entries(selectedValues).forEach(([pinKey, selectedValue]) => {
			if (selectedValue) {
				allPins.forEach((pin) => {
					if (pin.value !== selectedValue) {
						const option = pin.querySelector(`option[value="${selectedValue}"]`);
						if (option && option.value !== "inactive") {
							option.disabled = true;
							// Append the pin name to the label
							option.textContent = `${option.textContent} - ${pinKey.toUpperCase()} Pin`;
						}
					}
				});
			}
		});
	}

	// Function to initialize the analog settings dropdown functionality
	function initializeAnalogPinControls() {
		const rPin = document.getElementById("analog-led-r");
		const gPin = document.getElementById("analog-led-g");
		const bPin = document.getElementById("analog-led-b");
		const wwPin = document.getElementById("analog-led-ww");
		const cwPin = document.getElementById("analog-led-cw");

		const allPins = [rPin, gPin, bPin, wwPin, cwPin];

		// Add event listeners to update the options when a value is changed
		allPins.forEach((pin) => {
			pin.addEventListener("change", updateAnalogPinOptions);
		});

		// Initial call to update the options based on the default selections
		updateAnalogPinOptions();
	}

	// Initialize the analog settings section if mode is LED Strip
	const analogModeSelect = document.getElementById("analog-mode");
	analogModeSelect.addEventListener("change", function () {
		const selectedMode = analogModeSelect.value;
		if (selectedMode === "strip") {
			initializeAnalogPinControls();
		}
	});

	// Initially check the mode and initialize if already on LED Strip
	if (analogModeSelect.value === "strip") {
		initializeAnalogPinControls();
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

	// Toggle visibility of settings based on activation checkboxes
	toggleSection("wled-activate", ".wled-settings");
	toggleSection("analog-activate", ".analog-settings");
	toggleSection("switch-activate", ".switch-settings");

	// Initially hide all alerts
	hideAllAlerts();
	loadSettings();
});
