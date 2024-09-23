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

    // Show the success alert message
    const alertBox = document.getElementById('alert-saved-printer');
    alertBox.style.display = 'block';  // Show the alert

    // Hide the alert after 3 seconds
    setTimeout(() => {
        alertBox.style.display = 'none';
    }, 3000);
}

// Event listener for the Save button
document.getElementById('settings-printer-save').addEventListener('click', function(event) {
    event.preventDefault();  // Prevent form submission
    savePrinterSettings();    // Save the printer settings
});

// Load the printer settings when the page is loaded
window.onload = function () {
    loadPrinterSettings();
    
    // Initially hide the alert
    const alertBox = document.getElementById('alert-saved-printer');
    alertBox.style.display = 'none';
};
