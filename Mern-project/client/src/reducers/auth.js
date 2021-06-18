import { GOOGLE_LOGIN, LOGOUT, SIGN_IN, SIGN_UP  } from '../actions/types';


const initialState = {
    authData: null,
    isAuthenticated: false
}

const authReducer = (state = initialState, action) => {
    switch(action.type){
        case SIGN_UP:
        case SIGN_IN:
        case GOOGLE_LOGIN:
            localStorage.setItem('profile', JSON.stringify({ ...action?.payload }));
            return {
                ...state,
                authData: action?.payload,
                isAuthenticated: true
            }
        case LOGOUT:
            localStorage.removeItem('profile');
            return { ...state, authData: null, isAuthenticated: false }
        default:
            return state
    }
}

export default authReducer;