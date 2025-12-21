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

async function blockInfo() {
    const response = await fetch('http://localhost:8080/block_info', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({"block": "gr::basic::FunctionGenerator<float32>"})
    });

    const data = await response.json();
    console.log("Block parameters:", data.parameters);
    console.log("Block input ports:", data.inputs);
    console.log("Block output ports:", data.outputs);
}
