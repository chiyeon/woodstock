
const run_array_test_push = (n) => {
    let arr = []

    for (let i = 0; i < n; i++) {
        arr.push(i)
    }
}

const run_array_test_index = (n, arr) => {
    for (let i = 0; i < n; i++) {
        let a = arr[i]
    }
}

const run_array_test = (m, n) => {
    console.log("testing array of size " + n + " for " + m + " iterations.")
    let start_time = performance.now()
    for (let i = 0; i < m; i++) {
        run_array_test_push(n)
    }

    console.log("ADD:\ttook " + (performance.now() - start_time) / 1000 + " seconds")

    let arr = []
    for (let i = 0; i < m; i++) {
        arr.push(i)
    }

    start_time = performance.now()

    for (let i = 0; i < m; i++){
        
    }
}

const run_performance_test = () => {
    console.log("running array performance test")
    for (let i = 0; i < 10; i++) {
        run_array_test(1000 * i, 1000000)
    }
}

run_performance_test()

