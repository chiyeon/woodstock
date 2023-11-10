require('dotenv').config()

const app = require("express")()
const server = require("http").createServer(app)
const cors = require("cors")
const body_parser = require("body-parser")

const { initializeApp } = require("firebase/app")
const { getFirestore, doc, setDoc } = require("firebase/firestore")

const firebaseConfig = JSON.parse(process.env.FIREBASE_CONFIG)
const firebase_app = initializeApp(firebaseConfig)
const db = getFirestore(firebase_app)

const set_doc = async(col_name, doc_name, data) => {
   await setDoc(doc(db, col_name, doc_name), data)
}

const IP = "127.0.0.1"
const PORT = 3000

app.use(cors({ origin: [IP] }))
app.use(body_parser.json())

app.post("/upload", async (req, res) => {
   let pgn = req.body.pgn;
   let name = req.body.name;
   let id = Math.floor(Math.random() * 1000)

   let today = new Date()
   let dd = String(today.getDate()).padStart(2, '0');
   let mm = String(today.getMonth() + 1).padStart(2, '0');
   let yyyy = today.getFullYear();

   let date = `${mm}-${dd}-${yyyy}`

   await set_doc("games", `${name}_${date}_${id}`, {
      "pgn": pgn,
      "name": name,
      "date": date
   })
})

server.listen(PORT, async () => {

})