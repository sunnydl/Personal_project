import React, { useState } from 'react';
import { Avatar, Button, Paper, Grid, Typography, Container } from '@material-ui/core';
import { GoogleLogin } from 'react-google-login';
import useStyles from './styles';
import LockOutlinedIcon from '@material-ui/icons/LockOutlined';
import Input from './input';
import Icon from './icon';
import { useDispatch } from 'react-redux';
import { useHistory } from 'react-router-dom';
import { signUp, signIn } from '../../actions/auth';

const Auth = () => {
    const [showPassowrd, setShowPassword] = useState(false);
    const [isSignUp, setSignUp] = useState(false);
    const classes = useStyles();
    const dispatch = useDispatch();
    const history = useHistory();
    const [form, setForm] = useState({
        firstName: '',
        lastName: '',
        email: '',
        password: '',
        confirmPassword: ''
    })

    const handleSubmit = (e) => {
        e.preventDefault();
        if(isSignUp){
            dispatch(signUp(form, history))
        } else {
            dispatch(signIn(form, history))
        }
    };

    const handleChange = (e) => {
        setForm({ ...form, [e.target.name]: e.target.value })
    }

    const handleShowPassword = () => {
        setShowPassword(prev => !prev);
    }

    const switchMode = () => {
        setSignUp(prev => !prev);
        setShowPassword(false);
    };

    const googleSuccess = async (res) => {
        const result = res?.profileObj;
        const token = res?.tokenId;
        try{
            dispatch({ type: 'GOOGLE_LOGIN', payload: { result, token } });
            history.push('/');
        } catch(err){
            console.log(err);
        }
    }

    const googleFailure = () => {
        console.log("Cannot sign in to Google");
    }

    return (
        <Container component="main" maxWidth="xs">
            <Paper className={classes.paper} elevation={3}>
                <Avatar className={classes.avatar}>
                    <LockOutlinedIcon></LockOutlinedIcon>
                </Avatar>
                <Typography variant="h5">{isSignUp? 'Sign up': 'Sign in'}</Typography>
                <form className={classes.form} onSubmit={handleSubmit}>
                    <Grid container spacing={2}>
                        {
                            isSignUp && (
                                <>
                                    <Input name="firstName" label="First Name" handleChange={handleChange} autoFocus half></Input>
                                    <Input name="lastName" label="Last Name" handleChange={handleChange} autoFocus half></Input>
                                </>
                            )
                        }
                        <Input name="email" label="Email Address" handleChange={handleChange} type="email"></Input>
                        <Input name="password" label="Password" handleChange={handleChange} type={showPassowrd? "text": "password"} handleShowPassword={handleShowPassword}></Input>
                        {isSignUp && <Input name="confirmPassword" label="Repeat Password" handleChange={handleChange} type={showPassowrd? "text": "password"} />}
                    </Grid>
                    <Button type="submit" fullWidth variant="contained" color="primary" className={classes.submit}>{isSignUp? 'Sign up': 'Sign in'}</Button>
                    <GoogleLogin 
                        clientId="879496790019-isbk7c387059ude41h0klps5sc89miv8.apps.googleusercontent.com" 
                        render={(props) => (
                        <Button className={classes.googleButton} color="primary" fullWidth onClick={props.onClick} disabled={props.disabled} startIcon={<Icon/> } variant="contained">
                            Google Sign In
                        </Button>
                        )}
                        onSuccess={googleSuccess}
                        onFailure={googleFailure}
                        cookiePolicy="single_host_origin"
                    />
                    <Grid container justify="flex-end">
                        <Grid item>
                            <Button onClick={switchMode}>
                                {isSignUp? 'Already have account? Sign in': 'Dont have an account? Sign up'}
                            </Button>
                        </Grid>
                    </Grid>
                </form>
            </Paper>
        </Container>
    )
}

export default Auth
