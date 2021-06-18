import React from 'react';
import { Card, CardActions, CardContent, CardMedia, CardHeader, Button, Typography, Avatar } from '@material-ui/core/';
import ThumbUpAltIcon from '@material-ui/icons/ThumbUpAlt';
import DeleteIcon from '@material-ui/icons/Delete';
import MoreHorizIcon from '@material-ui/icons/MoreHoriz';
import moment from 'moment';
import { useDispatch } from 'react-redux';
import useStyles from './styles';
import { deletePost, likePost } from '../../../actions/posts';

const Post = ({ post, setCurrentId }) => {
  const dispatch = useDispatch();
  const classes = useStyles();
  const user = JSON.parse(localStorage.getItem('profile'))?.result;
  const showFeature = user && ((user?.googleId===post.creator)||(user?._id===post.creator));

  const handleDelete = (creator, id) => {
    if(user?.googleId){
      if(user.googleId===creator){
        dispatch(deletePost(id));
      }
    } else if(user?._id){
      if(user._id===creator){
        dispatch(deletePost(id));
      }
    }
  }

  return (
    <Card className={classes.card}>
      <CardHeader
        avatar={
          <Avatar aria-label="recipe" className={classes.avatar}>
            {post.name.charAt(0)}
          </Avatar>
        }
        title={
          <div>
            <Typography variant="h6">{post.name}</Typography>
            <Typography variant="body2">{moment(post.createdAt).fromNow()}</Typography>
          </div>
        }
      ></CardHeader>
      <CardMedia className={classes.media} image={post.selectedFile || 'https://user-images.githubusercontent.com/194400/49531010-48dad180-f8b1-11e8-8d89-1e61320e1d82.png'} title={post.title} />
      <div className={classes.overlay2}>
        {showFeature && <Button style={{ color: 'white' }} size="small" onClick={() => {setCurrentId(post._id)}}><MoreHorizIcon fontSize="default" /></Button>}
      </div>
      <div className={classes.details}>
        <Typography variant="body2" color="textSecondary" component="h2" style={{ fontFamily: 'Comic Sans MS' }}>{post.tags.map((tag) => `#${tag} `)}</Typography>
      </div>
      <Typography className={classes.title} gutterBottom variant="h5" component="h2" style={{ fontFamily: 'Comic Sans MS' }}>{post.title}</Typography>
      <CardContent>
        <Typography variant="body2" color="textSecondary" component="p" style={{ fontFamily: 'Comic Sans MS' }}>{post.message}</Typography>
      </CardContent>
      <CardActions className={classes.cardActions}>
        <Button size="small" color="primary" disabled={!user} onClick={() => {dispatch(likePost(post._id))}}><ThumbUpAltIcon fontSize="small" /> Like {post.likeCount} </Button>
        {showFeature && <Button size="small" color="primary" onClick={() => handleDelete(post.creator, post._id)}><DeleteIcon fontSize="small" /> Delete</Button>}
      </CardActions>
    </Card>
  );
};

export default Post;
