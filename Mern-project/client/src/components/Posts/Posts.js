import React from 'react';
import { Grid, CircularProgress } from '@material-ui/core';
import { useSelector } from 'react-redux';

import Post from './Post/Post';
import useStyles from './styles';

const Posts = ({ setCurrentId, tags }) => {
  const posts = useSelector((state) => state.posts);
  const classes = useStyles();

  // filter the posts if necessary
  const filtered = new Set();
  let filteredPosts = [];
  if(tags.length){
    // filter the posts according to the tags
    for(let i = 0; i < tags.length; i++){
      // compare the tags
      for(let j = 0; j < posts.length; j++){
        for(let o = 0; o < posts[j].tags.length; o++){
          if(tags[i]===posts[j].tags[o]){
            filtered.add(posts[j]);
            break;
          }
        }
      }
    }
    filteredPosts = Array.from(filtered)
  }

  return (
    !posts.length ? <CircularProgress /> : (
      <Grid className={classes.container} container alignItems="stretch" spacing={3}>
        {tags.length? filteredPosts.map((post) => (
          <Grid key={post._id} item xs={12} sm={6} md={6}>
            <Post post={post} setCurrentId={setCurrentId} />
          </Grid>
        )):
        posts.map((post) => (
          <Grid key={post._id} item xs={12} sm={6} md={6}>
            <Post post={post} setCurrentId={setCurrentId} />
          </Grid>
        ))}
      </Grid>
    )
  );
};

export default Posts;
