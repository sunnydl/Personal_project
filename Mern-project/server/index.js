import bodyParser from 'body-parser';
import express from 'express';
import mongoose from 'mongoose';
import cors from 'cors';
import dotenv from 'dotenv';

import postRoutes from './routes/posts.js';
import authRoutes from './routes/auth.js';

const app = express();
dotenv.config();

// config bodyParser so it can properly send request
app.use(bodyParser.json({ limit: "30mb", extended: true }));
app.use(bodyParser.urlencoded({ limit: "30mb", extended: true }));
app.use(cors());

app.use('/posts', postRoutes);
app.use('/auth', authRoutes);
app.get('/', (req, res) => {
    res.send('confirming deployment');
})


const CONNECTION_URL = process.env.CONNECTION_URL;
const PORT = process.env.PORT || 5000;

mongoose.connect(CONNECTION_URL, { useNewUrlParser: true, useUnifiedTopology: true, useCreateIndex: true }).then(() => {
    app.listen(PORT, () => {
        console.log(`Server running on: ${PORT}`)
    })
}).catch((err) => {
    console.log(err.message);
})

mongoose.set('useFindAndModify', false);