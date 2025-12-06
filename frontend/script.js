async function blockCount() {
    const response = await fetch('http://localhost:8080/block_count', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({})
    });

    const data = await response.json();
    console.log("Result from C++:", data.result);
}

async function blocks() {
    const response = await fetch('http://localhost:8080/blocks', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({})
    });

    const data = await response.json();
    console.log("Result from C++:", data.blocks);
}
