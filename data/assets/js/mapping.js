document.addEventListener("DOMContentLoaded", function () {
	// Load settings and mappings from localStorage
	const deviceSettings = JSON.parse(localStorage.getItem("deviceSettings")) || {};
	let mappings = JSON.parse(localStorage.getItem("mappings")) || [];

	const modal = new bootstrap.Modal(document.getElementById("modal-add-mapping"));
	const noMappingText = document.getElementById("no-mapping-yet");
	const mappingContainer = document.getElementById("mapping-container");
	const formMapping = document.getElementById("form-mapping");

	let currentMappingIndex = mappings.length;

	// Function to update "no mapping" text visibility
	function updateNoMappingText() {
		noMappingText.style.display = mappings.length === 0 ? "block" : "none";
	}

	// Function to create a new mapping card
	function addMappingCard(mapping, index) {
		const card = document.createElement("div");
		card.id = `card-mapping-${index}`;
		card.classList.add("card");
		card.style.marginBottom = "10px";

		// Get the readable labels from the <select> options
		const eventLabel = document.querySelector(`#mapping-event option[value='${mapping.event}']`).textContent;
		const outputLabel = document.querySelector(`#mapping-output option[value='${mapping.output}']`).textContent;
		const actionLabel = document.querySelector(`#mapping-action option[value='${mapping.action}']`).textContent;

		let additionalSettingsHtml = "";

		if (mapping.action === "color") {
			// Color-related settings, with color box aligned vertically and without a border
			additionalSettingsHtml = `
            <tr>
                <td>Selected Color</td>
                <td>
                    <div style="display: flex; align-items: center;">
                        <div style="display: inline-block; width: 20px; height: 20px; background-color: ${
													mapping.color
												}; margin-right: 10px;"></div>
                        <span>${mapping.color}</span>
                    </div>
                </td>
            </tr>
            <tr><td>Brightness</td><td>${mapping.brightness}%</td></tr>
            <tr><td>Blink</td><td>${mapping.blink ? "Yes" : "No"}</td></tr>
        `;

			if (mapping.blink) {
				additionalSettingsHtml += `
                <tr><td>Blink Duration</td><td>${mapping.blinkDuration}s</td></tr>
                <tr><td>Blink On/Off</td><td>On: ${mapping.blinkOn}ms, Off: ${mapping.blinkOff}ms</td></tr>
            `;
			}

			additionalSettingsHtml += `
            <tr><td>Turn Off After</td><td>${mapping.turnOff ? "Yes" : "No"}</td></tr>
        `;

			if (mapping.turnOff) {
				additionalSettingsHtml += `
                <tr><td>Turn Off Time</td><td>${mapping.turnOffTime}s</td></tr>
            `;
			}
		} else if (mapping.action === "pin-control") {
			// Pin-control-related settings
			additionalSettingsHtml = `
            <tr><td>Pin Control</td><td>${mapping.pinControl}</td></tr>
            <tr><td>Invert</td><td>${mapping.invert ? "Yes" : "No"}</td></tr>
        `;

			if (mapping.invert) {
				additionalSettingsHtml += `
                <tr><td>Invert Time</td><td>${mapping.invertTime}s</td></tr>
            `;
			}
		}

		// Build the card's inner HTML
		card.innerHTML = `
        <div class="card-body">
            <div class="row text-end">
                <div class="col text-start align-self-center">
                    <button id="btn-test-event-${index}" class="btn btn-secondary btn-sm" type="button">Test Event</button>
                </div>
                <div class="col align-self-center">
                    <button id="btn-mapping-delete-${index}" class="btn-close" type="button"></button>
                </div>
            </div>
            <div id="table-mapping-options-${index}" class="table-responsive">
                <table class="table table-hover table-sm">
                    <tbody>
                        <tr><td>Event</td><td>${eventLabel}</td></tr>
                        <tr><td>Output</td><td>${outputLabel}</td></tr>
                        <tr><td>Action</td><td>${actionLabel}</td></tr>
                        ${additionalSettingsHtml}
                    </tbody>
                </table>
            </div>
        </div>
    `;
		mappingContainer.appendChild(card);

		// Add event listener to delete button
		document.getElementById(`btn-mapping-delete-${index}`).addEventListener("click", function () {
			removeMappingCard(index);
		});
	}

	// Function to remove a mapping card
	function removeMappingCard(index) {
		document.getElementById(`card-mapping-${index}`).remove();
		mappings.splice(index, 1);
		localStorage.setItem("mappings", JSON.stringify(mappings));
		updateNoMappingText();
		updateMappingIndices();
	}

	// Function to update mapping card indices and ids
	function updateMappingIndices() {
		mappings.forEach((mapping, index) => {
			const card = document.getElementById(`card-mapping-${index}`);
			card.id = `card-mapping-${index}`;
		});
	}

	// Add saved mappings to the container on load
	mappings.forEach((mapping, index) => {
		addMappingCard(mapping, index);
	});

	// Add mapping button event listener
	document.getElementById("btn-add-mapping").addEventListener("click", function () {
		modal.show();
		updateFormBasedOnSettings(); // Reset form based on device settings
	});

	// Save button inside modal
	document.getElementById("btn-modal-mapping-save").addEventListener("click", function () {
		const event = formMapping.querySelector("#mapping-event").value;
		const output = formMapping.querySelector("#mapping-output").value;
		const action = formMapping.querySelector("#mapping-action").value;

		let newMapping = {
			event,
			output,
			action,
		};

		// Collect color-related settings if the action is "color"
		if (action === "color") {
			newMapping.color = formMapping.querySelector("#mapping-action-color-picker").value;
			newMapping.brightness = formMapping.querySelector("#mapping-action-color-brightness-slider").value;
			newMapping.blink = formMapping.querySelector("#mapping-action-color-blink-select").value === "true";
			if (newMapping.blink) {
				newMapping.blinkDuration = formMapping.querySelector("#mapping-action-color-blink-duration").value;
				newMapping.blinkOn = formMapping.querySelector("#mapping-action-color-blink-on").value;
				newMapping.blinkOff = formMapping.querySelector("#mapping-action-color-blink-off").value;
			}
			newMapping.turnOff = formMapping.querySelector("#mapping-action-color-turn-off").value === "true";
			if (newMapping.turnOff) {
				newMapping.turnOffTime = formMapping.querySelector("#mapping-action-color-turn-off-time").value;
			}
		}

		// Collect pin-control-related settings if the action is "pin-control"
		if (action === "pin-control") {
			newMapping.pinControl = formMapping.querySelector("#mapping-action-pin-control").value;
			newMapping.invert = formMapping.querySelector("#mapping-action-pin-invert").value === "true";
			if (newMapping.invert) {
				newMapping.invertTime = formMapping.querySelector("#mapping-action-pin-invert-time").value;
			}
		}

		// Add the new mapping to the array and update localStorage
		mappings.push(newMapping);
		localStorage.setItem("mappings", JSON.stringify(mappings));

		// Add a new card to the container
		addMappingCard(newMapping, currentMappingIndex);
		currentMappingIndex++;
		updateNoMappingText();
		resetModalForm();
		modal.hide();
	});

	// Syncing secondary settings in the color action
	const colorPicker = document.getElementById("mapping-action-color-picker");
	const colorText = document.getElementById("mapping-action-color-text");
	const brightnessText = document.getElementById("mapping-action-color-brightness-text");
	const brightnessSlider = document.getElementById("mapping-action-color-brightness-slider");

	colorPicker.addEventListener("input", function () {
		colorText.value = colorPicker.value;
	});

	colorText.addEventListener("input", function () {
		colorPicker.value = colorText.value;
	});

	brightnessSlider.addEventListener("input", function () {
		brightnessText.value = brightnessSlider.value;
	});

	brightnessText.addEventListener("input", function () {
		brightnessSlider.value = brightnessText.value;
	});

	// Toggle secondary settings visibility based on action and output
	function toggleSecondarySettings() {
		const output = document.getElementById("mapping-output").value;

		const colorContainer = document.getElementById("container-action-color");
		const pinControlContainer = document.getElementById("container-action-pin-control");

		// Hide both initially
		colorContainer.style.display = "none";
		pinControlContainer.style.display = "none";

		const pinControlOption = document.getElementById("mapping-action").querySelector("option[value='pin-control']");
		const colorOption = document.getElementById("mapping-action").querySelector("option[value='color']");

		// Show relevant container based on action and output
		if (output === "wled" || output === "analog-strip") {
			if (pinControlOption) pinControlOption.disabled = true;
			if (colorOption) colorOption.disabled = false;
		} else {
			if (pinControlOption) pinControlOption.disabled = false;
			if (colorOption) colorOption.disabled = true;
		}
	}

	document.getElementById("mapping-output").addEventListener("change", updateFormBasedOnSettings);
	document.getElementById("mapping-action").addEventListener("change", function () {
		const action = document.getElementById("mapping-action").value;
		const colorContainer = document.getElementById("container-action-color");
		const pinControlContainer = document.getElementById("container-action-pin-control");

		if (action === "color") {
			colorContainer.style.display = "block";
			pinControlContainer.style.display = "none";
		} else if (action === "pin-control") {
			colorContainer.style.display = "none";
			pinControlContainer.style.display = "block";
		} else {
			// Hide both containers if no valid action is selected
			colorContainer.style.display = "none";
			pinControlContainer.style.display = "none";
		}
	});

	// Blink select visibility toggle
	document.getElementById("mapping-action-color-blink-select").addEventListener("change", function () {
		const blink = this.value === "true";
		document.getElementById("mapping-action-color-blink-duration-group").style.display = blink ? "flex" : "none";
		document.getElementById("mapping-action-color-blink-on-off-group").style.display = blink ? "flex" : "none";
	});

	// Turn-off select visibility toggle
	document.getElementById("mapping-action-color-turn-off").addEventListener("change", function () {
		const turnOff = this.value === "true";
		document.getElementById("mapping-action-color-turn-off-time-group").style.display = turnOff ? "flex" : "none";
	});

	// Pin invert select visibility toggle
	document.getElementById("mapping-action-pin-invert").addEventListener("change", function () {
		const invert = this.value === "true";
		document.getElementById("mapping-action-pin-invert-time-group").style.display = invert ? "flex" : "none";
	});

	// Handling dynamic form settings based on localStorage configuration
	function updateFormBasedOnSettings() {
		const wledActive = deviceSettings.wled?.activate;
		const analogActive = deviceSettings.analog?.activate;
		const analogMode = deviceSettings.analog?.mode;
		const switchActive = deviceSettings.switch?.activate && deviceSettings.switch?.function === "event";

		// Disable WLED if it's inactive
		const wledOption = document.querySelector("#mapping-output option[value='wled']");
		if (wledOption) wledOption.disabled = !wledActive;

		// Handle analog LED activation
		const analogLedOptions = document.querySelectorAll("#mapping-output optgroup[label='Analog LED'] option");
		if (!analogActive) {
			analogLedOptions.forEach((opt) => {
				opt.disabled = true;
			});
		} else {
			// Handle analog settings based on mode
			if (analogMode === "individually") {
				const analogStripOption = document.querySelector("#mapping-output option[value='analog-strip']");
				if (analogStripOption) analogStripOption.disabled = true;

				analogLedOptions.forEach((opt) => {
					if (opt.value !== "analog-strip") {
						opt.disabled = false;
					}
				});
			} else if (analogMode === "strip") {
				const analogStripOption = document.querySelector("#mapping-output option[value='analog-strip']");
				if (analogStripOption) analogStripOption.disabled = false;

				analogLedOptions.forEach((opt) => {
					if (opt.value !== "analog-strip") {
						opt.disabled = true;
					}
				});
			}
		}

		// Disable onboard switch event if switch settings are inactive
		const switchOption = document.querySelector("#mapping-event option[value='switch']");
		if (switchOption) switchOption.disabled = !switchActive;

		// Set initial visibility based on default values
		toggleSecondarySettings();

		// Ensure the blink settings are hidden if blink is set to "no" by default
		const blinkSelect = document.getElementById("mapping-action-color-blink-select");
		const blink = blinkSelect.value === "false";
		document.getElementById("mapping-action-color-blink-duration-group").style.display = blink ? "none" : "flex";
		document.getElementById("mapping-action-color-blink-on-off-group").style.display = blink ? "none" : "flex";

		// Ensure turn-off settings are hidden if turn-off is set to "no" by default
		const turnOffSelect = document.getElementById("mapping-action-color-turn-off");
		const turnOff = turnOffSelect.value === "false";
		document.getElementById("mapping-action-color-turn-off-time-group").style.display = turnOff ? "none" : "flex";
	}

	// Function to reset the modal form after saving
	function resetModalForm() {
		// Reset the form's main select fields
		document.getElementById("mapping-event").value = "";
		document.getElementById("mapping-output").value = "";
		document.getElementById("mapping-action").value = "";

		// Reset color-related fields
		document.getElementById("mapping-action-color-text").value = "#000000";
		document.getElementById("mapping-action-color-picker").value = "#000000";
		document.getElementById("mapping-action-color-brightness-text").value = "20";
		document.getElementById("mapping-action-color-brightness-slider").value = "20";
		document.getElementById("mapping-action-color-blink-select").value = "false";
		document.getElementById("mapping-action-color-blink-duration").value = "";
		document.getElementById("mapping-action-color-blink-on").value = "";
		document.getElementById("mapping-action-color-blink-off").value = "";
		document.getElementById("mapping-action-color-turn-off").value = "false";
		document.getElementById("mapping-action-color-turn-off-time").value = "";

		// Reset pin-control-related fields
		document.getElementById("mapping-action-pin-control").value = "on";
		document.getElementById("mapping-action-pin-invert").value = "false";
		document.getElementById("mapping-action-pin-invert-time").value = "";

		// Hide secondary settings sections
		document.getElementById("container-action-color").style.display = "none";
		document.getElementById("container-action-pin-control").style.display = "none";

		// Reset any dynamically shown or hidden fields (like blink and invert time)
		document.getElementById("mapping-action-color-blink-duration-group").style.display = "none";
		document.getElementById("mapping-action-color-blink-on-off-group").style.display = "none";
		document.getElementById("mapping-action-color-turn-off-time-group").style.display = "none";
		document.getElementById("mapping-action-pin-invert-time-group").style.display = "none";
	}

	// Modal button close clicked
	document.getElementById("modal-add-mapping").addEventListener("hidden.bs.modal", function () {
		resetModalForm();
	});

	// Check for update
	document.getElementById("alert-update").style.display = "none";

	async function checkForUpdates() {
		const currentVersion = "0.1.2";
		const url = `https://api.github.com/repos/derDeno/BLWLED/releases/latest`;

		try {
			const response = await fetch(url);
			if (!response.ok) {
				console.log(`GitHub API returned an error: ${response.status}`);
			}

			const releaseData = await response.json();
			const latestVersion = releaseData.tag_name.replace(/^v/, ""); // Remove "v" if present

			if (isNewerVersion(latestVersion, currentVersion)) {
				console.log(`A newer version is available: ${latestVersion}`);
				document.getElementById("alert-update").style.display = "block";
			}

		} catch (error) {
			console.error(`Error checking for updates: ${error.message}`);
		}
	}

	function isNewerVersion(latest, current) {
		const latestParts = latest.split(".").map(Number);
		const currentParts = current.split(".").map(Number);

		for (let i = 0; i < latestParts.length; i++) {
			if (latestParts[i] > (currentParts[i] || 0)) {
				return true;
			} else if (latestParts[i] < (currentParts[i] || 0)) {
				return false;
			}
		}
		return false;
	}

	updateNoMappingText();
	updateFormBasedOnSettings();
	checkForUpdates();
});
