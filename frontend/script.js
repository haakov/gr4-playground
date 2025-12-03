async function callCppServer() {
    const response = await fetch('http://localhost:8080/rpc/add', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ x: 10, y: 20 })
    });

    const data = await response.json();
    console.log("Result from C++:", data.result);
}

async function callBlockCount() {
    const response = await fetch('http://localhost:8080/blocks/count', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({})
    });

    const data = await response.json();
    console.log("Result from C++:", data.result);
}
