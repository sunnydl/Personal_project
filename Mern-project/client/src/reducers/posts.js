import { GET_POSTS, ADD_POST, UPDATE_POST, DELETE_POST, LIKE_POST } from '../actions/types';

const postsReducer = (posts = [], action) => {
    switch(action.type){
        case GET_POSTS:
            return action.payload
        case LIKE_POST:
        case UPDATE_POST:
            return posts.map(post => post._id===action.payload._id? action.payload: post);
        case ADD_POST:
            return [...posts, action.payload];
        case DELETE_POST:
            return posts.filter(post => post._id!==action.payload._id)
        default:
            return posts;
    }
};

export default postsReducer;