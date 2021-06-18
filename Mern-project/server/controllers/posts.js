import mongoose from 'mongoose';
import PostMessage from '../models/postMessage.js';

export const getPosts = async (req, res) => {
    try{
        const postMessage = await PostMessage.find(); // finding something inside model takes time, which means its a async function
        res.status(200).json(postMessage);
    } catch(err) {
        res.status(404).json({ msg: err.message });
    }
}

export const createPost = async (req, res) => {
    const post = req.body;
    const newPost = new PostMessage({ ...post, creator: req.userId, createdAt: new Date().toISOString() });

    try{
        await newPost.save();
        res.status(201).json(newPost);
    } catch(err) {
        res.status(409).json({ msg: err.message })
    }
}

export const updatePost = async (req, res) => {
    const { id } = req.params;
    const { title, message, creator, selectedFile, tags } = req.body;
    
    if (!mongoose.Types.ObjectId.isValid(id)) return res.status(404).send(`No post with id: ${id}`);

    const updatedPost = { creator, title, message, tags, selectedFile, _id: id };

    await PostMessage.findByIdAndUpdate(id, updatedPost, { new: true });

    res.json(updatedPost);
}

export const deletePost = async (req, res) => {
    const { id } = req.params;

    if(!mongoose.Types.ObjectId.isValid(id)){
        return res.status(404).send('No post with such id');
    }
    try {
        const deletedPost = await PostMessage.findByIdAndDelete(id);
        res.json(deletedPost);
    } catch (err) {
        res.json({ msg: err.message })
    }
}

export const likePost = async (req, res) => {
    const { id } = req.params;

    if(!req.userId) {
        return res.json({ msg: "User is not authenticated" })
    }

    if(!mongoose.Types.ObjectId.isValid(id)){
        res.status(404).send('No post with such id');
    }
    try{
        const post = await PostMessage.findById(id);
        const index = post.likes.findIndex((id) => id===String(req.userId)); // find if the id is already in there
        // if id is already in there, then cannot like no more
        if(index===-1){
            post.likes.push(req.userId);
            post.likeCount = post.likeCount + 1;
        } else {
            post.likes = post.likes.filter(id => id!==String(req.userId));
            post.likeCount = post.likeCount - 1;
        }
        await PostMessage.findByIdAndUpdate(id, post, { new: true });
        res.json(post);
    } catch(err){
        res.status(500).json({ msg: err.message })
    }
}