document.addEventListener("DOMContentLoaded", function() {
    let mappingCount = 0;  // To keep track of how many mappings are created

    // Function to show the modal for adding a new mapping
    document.getElementById("btn-add-mapping").addEventListener("click", function() {
        const modal = new bootstrap.Modal(document.getElementById('modal-add-mapping'));
        modal.show();
    });

    // Function to handle saving a new mapping
    document.getElementById("btn-modal-mapping-save").addEventListener("click", function() {
        // Get the selected options (labels) from the modal form
        const outputSelect = document.getElementById("mapping-output");
        const outputTypeSelect = document.getElementById("mapping-output-type");
        const eventSelect = document.getElementById("mapping-event");

        const outputText = outputSelect.options[outputSelect.selectedIndex].text;
        const outputValue = outputSelect.value;

        const outputTypeText = outputTypeSelect.options[outputTypeSelect.selectedIndex].text;
        const outputTypeValue = outputTypeSelect.value;

        const eventText = eventSelect.options[eventSelect.selectedIndex].text;
        const eventValue = eventSelect.value;

        // Increment mapping count to create unique IDs for each card
        mappingCount += 1;

        // Create a new mapping card based on the user inputs
        const mappingCard = document.createElement("div");
        mappingCard.classList.add("card");
        mappingCard.id = `card-mapping-${mappingCount}`;
        mappingCard.innerHTML = `
            <div class="card-body" style="margin-bottom: 10px,">
                <div class="row text-end">
                    <div class="col text-start">
                        <h4>Mapping ${mappingCount}</h4>
                    </div>
                    <div class="col">
                        <button id="btn-mapping-delete-${mappingCount}" class="btn-close" type="button" aria-label="Close"></button>
                    </div>
                </div>
                <div class="input-group" style="margin-bottom: 10px;">
                    <span class="input-group-text">Output</span>
                    <input id="mapping-output-${mappingCount}" class="form-control" type="text" readonly value="${outputText}" data-output-value="${outputValue}" />
                </div>
                <div class="input-group" style="margin-bottom: 10px;">
                    <span class="input-group-text">Output Type</span>
                    <input id="mapping-output-type-${mappingCount}" class="form-control" type="text" readonly value="${outputTypeText}" data-output-type-value="${outputTypeValue}" />
                </div>
                <div class="input-group" style="margin-bottom: 10px;">
                    <span class="input-group-text">Event</span>
                    <input id="mapping-event-${mappingCount}" class="form-control" type="text" readonly value="${eventText}" data-event-value="${eventValue}" />
                </div>
            </div>
        `;

        // Append the new mapping card to the container
        document.getElementById("mapping-container").appendChild(mappingCard);

        // Hide the "No mapping added yet" message
        document.getElementById("no-mapping-yet").style.display = 'none';

        // Close the modal after saving
        const modal = bootstrap.Modal.getInstance(document.getElementById('modal-add-mapping'));
        modal.hide();

        // Add an event listener to delete the card when the close button is clicked
        document.getElementById(`btn-mapping-delete-${mappingCount}`).addEventListener("click", function() {
            const card = document.getElementById(`card-mapping-${mappingCount}`);
            card.remove();

            // If there are no mappings left, show the "No mapping added yet" message again
            if (document.getElementById("mapping-container").childElementCount === 0) {
                document.getElementById("no-mapping-yet").style.display = 'block';
            }
        });
    });

    // Handle dynamically removing the correct mapping card
    document.getElementById("mapping-container").addEventListener("click", function(event) {
        if (event.target.classList.contains("btn-close")) {
            const cardToDelete = event.target.closest(".card");
            cardToDelete.remove();

            // If there are no mappings left, show the "No mapping added yet" message again
            if (document.getElementById("mapping-container").childElementCount === 0) {
                document.getElementById("no-mapping-yet").style.display = 'block';
            }
        }
    });
});
