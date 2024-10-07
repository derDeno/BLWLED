// Function to load saved printer settings from the API and populate the form
async function loadPrinterSettings() {
    try {
        const response = await fetch('/api/settings-printer');
        if (response.ok) {
            const settings = await response.json();
            document.getElementById('settings-printer-ip').value = settings.printerIp || '';
            document.getElementById('settings-printer-access-code').value = settings.accessCode || '';
            document.getElementById('settings-printer-return-idle-door').value = settings.rtid ? 'true' : 'false';
            document.getElementById('settings-printer-return-idle-time').value = settings.rtit || '';
        } else {
            console.error('Failed to load printer settings');
        }
    } catch (error) {
        console.error('Error fetching printer settings:', error);
    }
}

// Function to save printer settings using the API
async function savePrinterSettings() {
    // Get the input values from the form
    const printerIP = document.getElementById('settings-printer-ip').value;
    const accessCode = document.getElementById('settings-printer-access-code').value;
    const returnToIdleDoor = document.getElementById('settings-printer-return-idle-door').value;
    const returnIdleTime = document.getElementById('settings-printer-return-idle-time').value;

    // Create form data to send to the API
    const formData = new URLSearchParams();
    formData.append('printerIp', printerIP);
    formData.append('accessCode', accessCode);
    formData.append('rtid', returnToIdleDoor === 'true');
    formData.append('rtit', returnIdleTime);

    try {
        const response = await fetch('/api/settings-printer', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
            body: formData.toString(),
        });

        if (response.ok) {
            // Show the success alert message for saving connection settings
            const alertBox = document.getElementById('alert-saved-printer-connection');
            alertBox.style.display = 'block';  // Show the alert

            // Hide the alert after 3 seconds
            setTimeout(() => {
                alertBox.style.display = 'none';
            }, 3000);
        } else {
            console.error('Failed to save printer settings');
        }
    } catch (error) {
        console.error('Error saving printer settings:', error);
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

// Event listener for "Test Connection" button to show the modal
document.getElementById('btn-connection-test').addEventListener('click', function () {
    showTestConnectionModal();  // Show the modal when the button is clicked
});

// Load the printer settings when the page is loaded
window.onload = function () {
    loadPrinterSettings();  // Load connection settings

    // Initially hide the success alert messages
    document.getElementById('alert-saved-printer-connection').style.display = 'none';
};
