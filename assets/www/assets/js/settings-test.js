document.addEventListener("DOMContentLoaded", function () {

    const colorPicker = document.getElementById("test-color-picker");
	const colorText = document.getElementById("test-color-text");

    colorPicker.addEventListener("input", function () {
		colorText.value = colorPicker.value;
	});

	colorText.addEventListener("input", function () {
		colorPicker.value = colorText.value;
	});
});


document.getElementById("btn-test-send").addEventListener("click", async function () {
    const colorText = document.getElementById("test-color-text");
    const output = document.getElementById("test-output");

	const formData = new URLSearchParams();
	formData.append("color", colorText.value);
    formData.append("output", output.value);

    try {
		const response = await fetch("/api/color-test", {
			method: "POST",
			headers: {
				"Content-Type": "application/x-www-form-urlencoded",
			},
			body: formData.toString(),
		});

		if (response.ok) {
			// nothing here yet
		} else {
			console.error("Failed to test color");
		}
	} catch (error) {
		console.error("Error sending test color:", error);
	}
});

document.getElementById("btn-test-off").addEventListener("click", async function () {

	const formData = new URLSearchParams();
	formData.append("turnoff", "");

    try {
		const response = await fetch("/api/color-test", {
			method: "POST",
			headers: {
				"Content-Type": "application/x-www-form-urlencoded",
			},
			body: formData.toString(),
		});

		if (response.ok) {
			// nothing here yet
		} else {
			console.error("Failed to turn off");
		}
	} catch (error) {
		console.error("Error sending turn off:", error);
	}
});