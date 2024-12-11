document.getElementById("btn-log-delete").addEventListener("click", async function (e) {
	e.preventDefault();
	try {
        const response = await fetch('/api/log-delete', {
            method: 'POST',
        });

        if (response.ok) {
            document.getElementById('alert-log-deleted').style.display = 'block';
            setTimeout(() => {
                document.getElementById('alert-log-deleted').style.display = 'none';
            }, 5000);
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
    // check if logging is diabled
    const containerLog = document.getElementById('container-log');
    const logContent = containerLog.innerText.trim();
    
    const btnDeleteLog = document.getElementById('btn-log-delete');
    const btnDownloadLog = document.getElementById('btn-log-download');
    
    if (logContent === 'Logging is disabled!') {
        btnDeleteLog.disabled = true;
        btnDownloadLog.disabled = true;
    }

    document.getElementById('alert-log-deleted').style.display = 'none';
};