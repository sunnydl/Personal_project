import axios from 'axios';

const API = axios.create({ baseURL: 'https://unspeakables.herokuapp.com/' })

API.interceptors.request.use((req) => {
    const profile = JSON.parse(localStorage.getItem('profile'));
    if(profile){
        req.headers.Authorization = `Bearer ${profile.token}`;
    }

    return req;
});

// post api

export const fetchPosts = () => API.get('/posts');

export const createPost = (newPost) => API.post('/posts', newPost);

export const updatePost = (id, newPost) => API.patch(`/posts/${id}`, newPost);

export const deletePost = (id) => API.delete(`/posts/${id}`);

export const likePost = (id) => API.patch(`/posts/${id}/likePost`);


// auth api

export const signIn = (user) => API.post('/auth/signIn', user);

export const signUp = (user) => API.post('/auth/signUp', user);