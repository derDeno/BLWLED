document.addEventListener("DOMContentLoaded", function () {
    // Load settings and mappings from localStorage
    const deviceSettings = JSON.parse(localStorage.getItem("deviceSettings")) || {};
    let mappings = JSON.parse(localStorage.getItem("mappings")) || [];

    const modal = new bootstrap.Modal(document.getElementById('modal-add-mapping'));
    const noMappingText = document.getElementById("no-mapping-yet");
    const mappingContainer = document.getElementById("mapping-container");
    const formMapping = document.getElementById("form-mapping");

    let currentMappingIndex = mappings.length;

    // Function to update no mapping text visibility
    function updateNoMappingText() {
        noMappingText.style.display = mappings.length === 0 ? "block" : "none";
    }

    // Function to create a new mapping card
    function addMappingCard(mapping, index) {
        const card = document.createElement("div");
        card.id = `card-mapping-${index}`;
        card.classList.add("card");
        card.style.marginBottom = "10px";

        card.innerHTML = `
            <div class="card-body">
                <div class="row text-end">
                    <div class="col text-start align-self-center"><button id="btn-test-event-${index}" class="btn btn-secondary btn-sm" type="button">Test Event</button></div>
                    <div class="col align-self-center"><button id="btn-mapping-delete-${index}" class="btn-close" type="button"></button></div>
                </div>
                <div id="table-mapping-options-${index}" class="table-responsive">
                    <table class="table table-hover table-sm">
                        <tbody>
                            <tr>
                                <td>Event</td>
                                <td>${mapping.event}</td>
                            </tr>
                            <tr>
                                <td>Output</td>
                                <td>${mapping.output}</td>
                            </tr>
                            <tr>
                                <td>Action</td>
                                <td>${mapping.action}</td>
                            </tr>
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

    updateNoMappingText();

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

        const newMapping = {
            event,
            output,
            action
        };

        // Add the new mapping to the array and update localStorage
        mappings.push(newMapping);
        localStorage.setItem("mappings", JSON.stringify(mappings));

        // Add a new card to the container
        addMappingCard(newMapping, currentMappingIndex);
        currentMappingIndex++;
        updateNoMappingText();
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
        const action = document.getElementById("mapping-action").value;
        const output = document.getElementById("mapping-output").value;
        const analogMode = deviceSettings.analog?.mode;

        const colorContainer = document.getElementById("container-action-color");
        const pinControlContainer = document.getElementById("container-action-pin-control");

        // Hide both initially
        colorContainer.style.display = "none";
        pinControlContainer.style.display = "none";

        // Show relevant container based on action and output
        if (output === "wled") {
            document.getElementById("mapping-action option[value='pin-control']").disabled = true;
            document.getElementById("mapping-action option[value='color']").disabled = false;
            document.getElementById("mapping-action").value = "color";
            colorContainer.style.display = "block";
        } else if (output.startsWith("GPIO") || (output.startsWith("analog") && analogMode === "tower")) {
            document.getElementById("mapping-action option[value='pin-control']").disabled = false;
            document.getElementById("mapping-action option[value='color']").disabled = true;
            document.getElementById("mapping-action").value = "pin-control";
            pinControlContainer.style.display = "block";
        } else if (output.startsWith("analog") && analogMode === "strip") {
            document.getElementById("mapping-action option[value='color']").disabled = false;
            document.getElementById("mapping-action option[value='pin-control']").disabled = true;
            document.getElementById("mapping-action").value = "color";
            colorContainer.style.display = "block";
        }
    }

    document.getElementById("mapping-action").addEventListener("change", toggleSecondarySettings);
    document.getElementById("mapping-output").addEventListener("change", function () {
        updateFormBasedOnSettings();
        toggleSecondarySettings();
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
        const analogMode = deviceSettings.analog?.mode;
        const analogLedSettings = analogMode === "tower" ? deviceSettings.analog?.tower : deviceSettings.analog?.led;
        const switchActive = deviceSettings.switch?.activate && deviceSettings.switch?.function === 'event';

        // Disable WLED if it's inactive
        document.querySelector("#mapping-output option[value='wled']").disabled = !wledActive;

        // Handle analog settings based on mode and LED/tower settings
        if (analogMode === "strip") {
            document.querySelectorAll("#mapping-output optgroup[label='Analog LED'] option").forEach(opt => {
                opt.disabled = false;
            });
        } else if (analogMode === "tower") {
            document.querySelector("#mapping-output option[value='analog-strip']").disabled = true;
            document.querySelectorAll("#mapping-output optgroup[label='Analog LED'] option").forEach(opt => {
                opt.disabled = true;
            });
        }

        // Disable analog LEDs/tower pins if any specific pin is inactive
        if (analogLedSettings) {
            for (const pin in analogLedSettings) {
                if (analogLedSettings[pin] === 'inactive') {
                    document.querySelector(`#mapping-output option[value='analog-${pin}']`).disabled = true;
                }
            }
        }

        // Disable onboard switch event if switch settings are inactive
        document.querySelector("#mapping-event option[value='switch']").disabled = !switchActive;

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

    updateFormBasedOnSettings();
});
