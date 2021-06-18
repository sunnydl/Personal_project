import User from '../models/authUser.js';
import bcrypt from 'bcryptjs';
import jwt from 'jsonwebtoken';


export const signIn = async (req, res) => {
    const { email, password } = req.body;
    
    try{
        const user = await User.findOne({ email });
        if(!user){
            return res.status(404).json({ msg: "Cannot find user with such email" });
        }
        const isPasswordCorrect = await bcrypt.compare(password, user.password); // we cannot compare them through string cuz in the register/signUp we have hashed the password
        if(!isPasswordCorrect){
            return res.status(403).json({ msg: "Password incorrect" });
        }
        const token = jwt.sign({ email: user.email, id: user._id }, process.env.jwtTest, { expiresIn: '1h' });

        res.status(200).json({ result: user, token });
    } catch(err){
        res.status(500).json({ msg: err.message })
    }
}

export const signUp = async (req, res) => {
    const { email, password, firstName, lastName, confirmPassword } = req.body;

    try{
        const foundUser = await User.findOne({ email });
        if(foundUser){
            return res.status(400).json({ msg: "User with this email already exists" })
        }
        if(password!==confirmPassword){
            return res.status(400).json({ msg: "password not matched" })
        }
        const hashedPassword = await bcrypt.hash(password, 12);

        const user = await User.create({
            email: email,
            password: hashedPassword,
            name: `${firstName} ${lastName}`
        })
        const token = jwt.sign({ email: user.email, id: user._id }, process.env.jwtTest, { expiresIn: '1h' });
        res.status(200).json({ result: user, token });
    } catch(err) {
        res.status(500).json({ msg: err.message })
    }
}