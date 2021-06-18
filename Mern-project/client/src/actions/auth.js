import { SIGN_UP, SIGN_IN } from './types';
import * as api from '../api';

export const signUp = (form, history) => async (dispatch) => {
    try{
        const { data } = await api.signUp(form);
        dispatch({
            type: SIGN_UP,
            payload: data
        });
        history.push('/');
    } catch(err) {
        console.log(err);
    }
}

export const signIn = (form, history) => async (dispatch) => {
    try{
        const { data } = await api.signIn(form);
        dispatch({
            type: SIGN_IN,
            payload: data
        })
        history.push('/');
    } catch (err) {
        console.log(err);
        if(err.message==='Request failed with status code 404'){
            // meaning no account was found
            alert("account does not exist");
        } else if(err.message==='Request failed with status code 403'){
            // wrong user name or password
            alert("wrong username or password");
        }
    }
}