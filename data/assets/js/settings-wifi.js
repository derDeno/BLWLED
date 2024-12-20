document.addEventListener('DOMContentLoaded', () => {
    const networkContainer = document.getElementById('network-container');
    const spinnerNetworkScan = document.getElementById('spinner-network-scan');
    const btnScanNetwork = document.getElementById('btn-scan-network');
    const listGroup = document.getElementById('network-group');
    const modalNetworkDetails = new bootstrap.Modal(document.getElementById('modal-network-details'));
    const modalConnect = new bootstrap.Modal(document.getElementById('modal-network-connect'));
    const modalNetworkSuccess = new bootstrap.Modal(document.getElementById('modal-network-success'));
    const modalNetworkFailed = new bootstrap.Modal(document.getElementById('modal-network-failed'));
    const networkNameField = document.getElementById('network-ssid');
    const networkConnectButton = document.getElementById('network-connect');
    const passwordField = document.getElementById('network-passphrase');

    // Initially hide the network-container and spinner
    networkContainer.style.display = 'none';
    spinnerNetworkScan.style.display = 'none';

    // Event listener for scan button
    btnScanNetwork.addEventListener('click', async () => {
        // Hide network container, show spinner, and clear network list
        networkContainer.style.display = 'none';
        spinnerNetworkScan.style.display = 'block';
        btnScanNetwork.style.display = 'none';
        listGroup.innerHTML = '';

        try {
            const eventSource = new EventSource('/api/events');
            
            // Call scan API
            await fetch('/api/wifi/scan');

            const networks = [];

            eventSource.addEventListener('network', (event) => {
                const networkData = JSON.parse(event.data);
                networks.push(networkData);
            });

            // Wait a few seconds for incoming messages
            setTimeout(() => {
                spinnerNetworkScan.style.display = 'none';
                btnScanNetwork.style.display = 'block';
                eventSource.close();

                // Populate network list
                networks.forEach((network, index) => {
                    const item = document.createElement('div');
                    item.className = 'list-group-item network-item';
                    item.id = `network-item-${index + 1}`;
                    item.textContent = `${network.ssid} (RSSI: ${network.rssi})`;

                    // Attach click event to item
                    item.addEventListener('click', () => {
                        networkNameField.value = network.ssid;
                        modalNetworkDetails.show();

                        if (network.encryption === 'Open') {
                            passwordField.disabled = true;
                            passwordField.value = '';
                            networkConnectButton.disabled = false;
                        } else if (network.encryption === 'Secured') {
                            passwordField.disabled = false;
                            passwordField.value = '';
                            networkConnectButton.disabled = true;

                            // Enable connect button only when all fields are filled and password is valid
                            const formFields = document.querySelectorAll('.modal-body input');
                            formFields.forEach((field) => {
                                field.addEventListener('input', () => {
                                    const allFieldsFilled = Array.from(formFields).every(f => f.value.trim() !== '');
                                    const isPasswordValid = passwordField.value.trim().length >= 8;
                                    networkConnectButton.disabled = !(allFieldsFilled && isPasswordValid);
                                });
                            });
                        }
                    });

                    listGroup.appendChild(item);
                });

                // Add 'Other Network' option at the end
                const otherNetworkItem = document.createElement('li');
                otherNetworkItem.className = 'list-group-item';
                otherNetworkItem.id = 'network-item-other';
                otherNetworkItem.innerHTML = '<span>Other Network</span>';

                otherNetworkItem.addEventListener('click', () => {
                    networkNameField.value = '';
                    passwordField.disabled = false;
                    passwordField.value = '';
                    networkConnectButton.disabled = true;
                    modalNetworkDetails.show();

                    const formFields = document.querySelectorAll('.modal-body input');
                    formFields.forEach((field) => {
                        field.addEventListener('input', () => {
                            const allFieldsFilled = Array.from(formFields).every(f => f.value.trim() !== '');
                            const isPasswordValid = passwordField.value.trim().length >= 8;
                            networkConnectButton.disabled = !(allFieldsFilled && isPasswordValid);
                        });
                    });
                });

                listGroup.appendChild(otherNetworkItem);

                // Show network container
                networkContainer.style.display = 'block';
            }, 5000);

        } catch (error) {
            console.error('Error during network scan:', error);
            spinnerNetworkScan.style.display = 'none';
            btnScanNetwork.style.display = 'block';
        }
    });

    // Event listener for connect button
    networkConnectButton.addEventListener('click', async () => {
        modalNetworkDetails.hide();
        modalConnect.show();

        const ssid = networkNameField.value;
        const pw = passwordField.value;

        try {
            const response = await fetch('/api/wifi/change', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: new URLSearchParams({ ssid, pw }),
            });

            const result = await response.json();

            if (result.status === 'initiated') {
                const eventSource = new EventSource('/api/events');

                eventSource.addEventListener('network-switch', (event) => {
                    const eventData = event.data;
                    modalConnect.hide();
                    if (eventData === 'success') {
                        modalNetworkSuccess.show();
                        setTimeout(() => {
                            location.reload();
                        }, 8000);
                    } else if (eventData === 'failed') {
                        modalNetworkFailed.show();
                    }
                    eventSource.close();
                });
            }
        } catch (error) {
            console.error('Error during connection attempt:', error);
            modalConnect.hide();
            modalNetworkFailed.show();
        }
    });
});
