document.addEventListener("DOMContentLoaded", function() {
    let mappingCount = 0;  // To keep track of total mappings created

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

        // Create a new card title by combining Output and Event
        const mappingTitle = `${outputText}: ${eventText}`;

        // Create a new mapping card with the generated title and add margin-bottom: 10px
        mappingCount += 1;  // Increment mapping count
        const mappingCard = document.createElement("div");
        mappingCard.classList.add("card");
        mappingCard.id = `card-mapping-${mappingCount}`;
        mappingCard.style.marginBottom = '10px';  // Add margin-bottom here
        mappingCard.innerHTML = `
            <div class="card-body">
                <div class="row text-end">
                    <div class="col text-start">
                        <h4>${mappingTitle}</h4>
                    </div>
                    <div class="col">
                        <button class="btn-close btn-mapping-delete" type="button" aria-label="Close" data-id="${mappingCount}"></button>
                    </div>
                </div>
                <div class="input-group" style="margin-bottom: 10px;">
                    <span class="input-group-text">Output</span>
                    <input class="form-control" type="text" readonly value="${outputText}" data-value="${outputValue}" />
                </div>
                <div class="input-group" style="margin-bottom: 10px;">
                    <span class="input-group-text">Output Type</span>
                    <input class="form-control" type="text" readonly value="${outputTypeText}" data-value="${outputTypeValue}" />
                </div>
                <div class="input-group" style="margin-bottom: 10px;">
                    <span class="input-group-text">Event</span>
                    <input class="form-control" type="text" readonly value="${eventText}" data-value="${eventValue}" />
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
    });

    // Handle dynamically removing the correct mapping card
    document.getElementById("mapping-container").addEventListener("click", function(event) {
        if (event.target.classList.contains("btn-mapping-delete")) {
            const cardId = event.target.getAttribute("data-id");
            const cardToDelete = document.getElementById(`card-mapping-${cardId}`);
            cardToDelete.remove();

            // Decrease mapping count and check if "No mapping added yet" should be displayed
            mappingCount -= 1;

            if (mappingCount === 0) {
                document.getElementById("no-mapping-yet").style.display = 'block';
            }
        }
    });
});
