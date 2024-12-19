// Function to load saved printer settings from the API and populate the form
async function loadPrinterSettings() {
    try {
        const response = await fetch('/api/settings/printer');
        if (response.ok) {
            const settings = await response.json();
            document.getElementById('settings-printer-ip').value = settings.printerIp || '';
            document.getElementById('settings-printer-access-code').value = settings.accessCode || '';
            document.getElementById('settings-printer-sn').value = settings.sn || '';
            document.getElementById('settings-printer-return-idle-door').value = settings.rtid ? 'true' : 'false';
            document.getElementById('settings-printer-return-standby-time').value = settings.rtsb || '';
        } else {
            console.error('Failed to load printer settings');
        }
    } catch (error) {
        console.error('Error fetching printer settings:', error);
    }
}

// Function to save printer settings using the API
async function savePrinterSettings() {
    const printerIP = document.getElementById('settings-printer-ip').value;
    const accessCode = document.getElementById('settings-printer-access-code').value;
    const sn = document.getElementById('settings-printer-sn').value;
    const returnToIdleDoor = document.getElementById('settings-printer-return-idle-door').value;
    const returnStandByTime = document.getElementById('settings-printer-return-standby-time').value;

    // Create form data to send to the API
    const formData = new URLSearchParams();
    formData.append('ip', printerIP);
    formData.append('ac', accessCode);
    formData.append('sn', sn);
    formData.append('rtid', returnToIdleDoor === 'true');
    formData.append('rtsb', returnStandByTime);

    try {
        const response = await fetch('/api/settings/printer', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
            body: formData.toString(),
        });

        if (response.ok) {
            const alertBox = document.getElementById('alert-saved-printer-settings');
            alertBox.style.display = 'block';

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

async function showTestConnectionModal() {
    document.getElementById('modal-test-connection-testing').style.display = 'block';
    document.getElementById('modal-test-connection-success').style.display = 'none';
    document.getElementById('modal-test-connection-fail').style.display = 'none';

    const testConnectionModal = new bootstrap.Modal(document.getElementById('modal-test-connection'));
    testConnectionModal.show();

    // run connection test
    try {
        const response = await fetch('/api/test/printer');
        if (response.ok) {
            const res = await response.json();

            document.getElementById('modal-test-connection-testing').style.display = 'none';

            if (res.status === 'success') {
                document.getElementById('modal-test-connection-success').style.display = 'block';
                document.getElementById('modal-test-connection-fail').style.display = 'none';
            } else {
                document.getElementById('modal-test-connection-success').style.display = 'none';
                document.getElementById('modal-test-connection-fail').style.display = 'block';
            }
        } else {
            console.error('Failed to connect to printer');
        }
    } catch (error) {
        console.error('Error calling API:', error);
    }
}

document.getElementById('btn-modal-test-connection-close').addEventListener('click', function () {
    const testConnectionModal = bootstrap.Modal.getInstance(document.getElementById('modal-test-connection'));
    testConnectionModal.hide();
});

document.getElementById('settings-printer-save').addEventListener('click', function (event) {
    event.preventDefault();
    savePrinterSettings();
});

document.getElementById('btn-connection-test').addEventListener('click', function () {
    showTestConnectionModal();
});

window.onload = function () {
    loadPrinterSettings();
    document.getElementById('alert-saved-printer-settings').style.display = 'none';
    document.getElementById('modal-test-connection-success').style.display = 'none';
    document.getElementById('modal-test-connection-fail').style.display = 'none';
};
