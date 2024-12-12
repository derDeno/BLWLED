// capture the click of the btn-reboot button and show the modal modal-reboot
document.getElementById("btn-reboot").addEventListener("click", async function (e) {
    e.preventDefault();
    const modal = new bootstrap.Modal(document.getElementById("modal-reboot"));
    modal.show();

    try {
        await fetch("/api/reboot", {
            method: "POST",
        });

        const checkDeviceStatus = async () => {
            try {
                const controller = new AbortController();
                const timeoutId = setTimeout(() => controller.abort(), 800);

                const response = await fetch("/api/info", { signal: controller.signal });
                clearTimeout(timeoutId);

                if (response.ok) {
                    const modal = bootstrap.Modal.getInstance(document.getElementById("modal-reboot"));
                    modal.hide();
                    location.reload();
                } else {
                    console.error("Device is not reachable yet");
                    setTimeout(checkDeviceStatus, 1000);
                }
            } catch (error) {
                console.error("Error checking device status:", error);
                setTimeout(checkDeviceStatus, 1000);
            }
        };

        setTimeout(checkDeviceStatus, 1000);
    } catch (error) {
        console.error("Error rebooting device:", error);
    }
});