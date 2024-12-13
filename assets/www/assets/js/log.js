document.getElementById("btn-log-delete").addEventListener("click", async function (e) {
    e.preventDefault();
    try {
        const response = await fetch('/api/log-delete', {
            method: 'POST',
        });

        if (response.ok) {
            window.location.href = window.location.pathname + '?logDeleted=true';
        } else {
            console.error('Failed to delete log');
        }
    } catch (error) {
        console.error('Error deleting log:', error);
    }
});

document.getElementById("btn-log-download").addEventListener("click", function (e) {
    e.preventDefault();
    window.open('/api/log-download/', '_blank');
});

window.onload = function () {
    // check if logging is disabled
    const containerLog = document.getElementById('container-log');
    const logContent = containerLog.innerText.trim();
    
    const btnDeleteLog = document.getElementById('btn-log-delete');
    const btnDownloadLog = document.getElementById('btn-log-download');
    
    if (logContent === 'Logging is disabled!') {
        btnDeleteLog.disabled = true;
        btnDownloadLog.disabled = true;
    }

    // Check URL parameters for logDeleted
    const urlParams = new URLSearchParams(window.location.search);
    if (urlParams.get('logDeleted') === 'true') {
        document.getElementById('alert-log-deleted').style.display = 'block';
        setTimeout(() => {
            document.getElementById('alert-log-deleted').style.display = 'none';
        }, 5000);
        
        // Remove the parameter from the URL without reloading the page
        urlParams.delete('logDeleted');
        window.history.replaceState({}, document.title, window.location.pathname);
    } else {
        document.getElementById('alert-log-deleted').style.display = 'none';
    }
};