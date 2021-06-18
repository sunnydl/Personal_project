import { GET_POSTS, ADD_POST, UPDATE_POST, DELETE_POST, LIKE_POST } from './types';
import * as api from '../api';


export const getPosts = () => async (dispatch) => {
    try{
        const { data } = await api.fetchPosts();
        dispatch({
            type: GET_POSTS,
            payload: data
        })
    } catch(err) {
         console.log(err.message);
    }
}

export const createPost = (newPost) => async (dispatch) => {
    try{
        const { data } = await api.createPost(newPost);
        dispatch({
            type: ADD_POST,
            payload: data
        })
    } catch(err) {
        console.log(err);
    }
}

export const updatePost = (id, post) => async (dispatch) => {
    try {
      const { data } = await api.updatePost(id, post);
  
      dispatch({ type: UPDATE_POST, payload: data });
    } catch (error) {
      console.log(error.message);
    }
};

export const deletePost = (id) => async (dispatch) => {
    try {
        const { data } = await api.deletePost(id);
        dispatch({
            type: DELETE_POST,
            payload: data
        })
    } catch(err){
        console.log(err);
    }
}

export const likePost = (id) => async (dispatch) => {
    try{
        const { data } = await api.likePost(id);
        dispatch({
            type: LIKE_POST,
            payload: data
        })
    } catch(err) {
        console.log(err);
    }
}