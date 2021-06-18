import jwt from 'jsonwebtoken';

const auth = async (req, res, next) => {  
    try{
        const token = req.headers.authorization.split(" ")[1];
        const isCustomAuth = token.length < 500;
        let decodedData;
        if(token && isCustomAuth){
            // our own token
            decodedData = jwt.verify(token, process.env.jwtTest) // get the object back from the token
            req.userId = decodedData?.id; // store the id inside the req and pass that
        } else if(token && !isCustomAuth){
            // google token
            decodedData = jwt.decode(token); // get the object back from the token
            req.userId = decodedData?.sub; // store the id from google token inside the req and pass that
        }
        next();
    } catch(err) {
        console.log(err);
    }
}

export default auth;