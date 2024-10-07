// Function to load saved printer settings from localStorage and populate the form
function loadPrinterSettings() {
    // Get saved settings (as JSON string) from localStorage
    const savedSettings = localStorage.getItem('printerSettings');

    // If settings exist, parse the JSON and populate the form fields
    if (savedSettings) {
        const settings = JSON.parse(savedSettings);
        document.getElementById('settings-printer-ip').value = settings.printerIP || '';
        document.getElementById('settings-printer-access-code').value = settings.accessCode || '';
    }
}

// Function to save printer settings as a JSON string
function savePrinterSettings() {
    // Get the input values from the form
    const printerIP = document.getElementById('settings-printer-ip').value;
    const accessCode = document.getElementById('settings-printer-access-code').value;

    // Create an object with the settings
    const settings = {
        printerIP: printerIP,
        accessCode: accessCode
    };

    // Save the settings object as a JSON string in localStorage
    localStorage.setItem('printerSettings', JSON.stringify(settings));

    // Show the success alert message for saving connection settings
    const alertBox = document.getElementById('alert-saved-printer-connection');
    alertBox.style.display = 'block';  // Show the alert

    // Hide the alert after 3 seconds
    setTimeout(() => {
        alertBox.style.display = 'none';
    }, 3000);
}

// Function to save event settings as a JSON string
function saveEventSettings() {
    // Get the input values from the form
    const returnToIdleDoor = document.getElementById('settings-printer-return-idle-door').value;
    const returnIdleTime = document.getElementById('settings-printer-return-idle-time').value;

    // Create an object with the event settings
    const eventSettings = {
        returnToIdleDoor: returnToIdleDoor,
        returnIdleTime: returnIdleTime
    };

    // Save the event settings object as a JSON string in localStorage
    localStorage.setItem('printerEventSettings', JSON.stringify(eventSettings));

    // Show the success alert message for saving event settings
    const alertBox = document.getElementById('alert-saved-printer-events');
    alertBox.style.display = 'block';  // Show the alert

    // Hide the alert after 3 seconds
    setTimeout(() => {
        alertBox.style.display = 'none';
    }, 3000);
}

// Function to load saved event settings from localStorage and populate the form
function loadEventSettings() {
    const savedEventSettings = localStorage.getItem('printerEventSettings');

    // If event settings exist, parse and populate the form fields
    if (savedEventSettings) {
        const eventSettings = JSON.parse(savedEventSettings);
        document.getElementById('settings-printer-return-idle-door').value = eventSettings.returnToIdleDoor || 'true';
        document.getElementById('settings-printer-return-idle-time').value = eventSettings.returnIdleTime || '';
    }
}

// Function to handle the modal visibility for testing printer connection
function showTestConnectionModal() {
    // Use Bootstrap's modal API to show the modal
    const testConnectionModal = new bootstrap.Modal(document.getElementById('modal-test-connection'));
    testConnectionModal.show();
}

// Event listener to close the modal when "Close" button is clicked
document.getElementById('btn-modal-test-connection-close').addEventListener('click', function () {
    const testConnectionModal = bootstrap.Modal.getInstance(document.getElementById('modal-test-connection'));
    testConnectionModal.hide();
});

// Event listener for the Save button (Printer Connection)
document.getElementById('settings-printer-connection-save').addEventListener('click', function (event) {
    event.preventDefault();  // Prevent form submission
    savePrinterSettings();    // Save the printer connection settings
});

// Event listener for the Save button (Printer Event Settings)
document.getElementById('settings-printer-events-save').addEventListener('click', function (event) {
    event.preventDefault();  // Prevent form submission
    saveEventSettings();    // Save the printer event settings
});

// Event listener for "Test Connection" button to show the modal
document.getElementById('btn-connection-test').addEventListener('click', function () {
    showTestConnectionModal();  // Show the modal when the button is clicked
});

// Load the printer settings when the page is loaded
window.onload = function () {
    loadPrinterSettings();  // Load connection settings
    loadEventSettings();    // Load event settings

    // Initially hide the success alert messages
    document.getElementById('alert-saved-printer-connection').style.display = 'none';
    document.getElementById('alert-saved-printer-events').style.display = 'none';
};
