const test_add = (N, obj) => {
    for (let i = 0; i < N; i++) {
        obj[i] = i * i
    }

    return obj
}

const test_index = (N, obj) => {
    for (let i = 0; i < N; i++) {
        let a = obj[i]
        a += 1
    }
}

const test_obj = (obj, name) => {
    //console.log("Testing performance of " + name)

    let objs = []
    let o
    let results = []

    //console.log("testing add")
    let start_time = performance.now()
    //for (let i = 1; i <= 10; i++) {
        //for (let j = 0; j < i * 1000; j++) {
            o = test_add(1000000, obj)
        //}
    //}
    let time = (performance.now() - start_time) / 1000
    results.push(time)
    //console.log("took " + time)

    //console.log("testing index")
    start_time = performance.now()
    //for (let i = 1; i <= 10; i++) {
        //for (let j = 0; j < i * 1000; j++) {
            test_index(1000000, o)
        //}
    //}
    time = (performance.now() - start_time) / 1000
    results.push(time)
    //console.log("took " + time)

    return results
}

const full_test_obj = (N, obj, name) => {
    let avg_add = 0
    let avg_index = 0
    for (let i = 0; i < N; i++) {
        let res = test_obj(obj, name)
        avg_add += res[0]
        avg_index += res[1]
    }

    avg_add /= N
    avg_index /= N

    console.log("------------------------------")
    console.log("Adding took an average of " + avg_add + " seconds (N = " + N + ")")
    console.log("Indexing took an average of " + avg_index + " seconds (N = " + N + ")")
}

full_test_obj(100, [], "array")
full_test_obj(100, {}, "object")