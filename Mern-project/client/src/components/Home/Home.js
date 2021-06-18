import React, { useEffect, useState } from 'react';
import { Container, Grow, Grid } from '@material-ui/core';
import Posts from '../Posts/Posts';
import Form from '../Form/Form';
import { useDispatch } from 'react-redux';
import { getPosts } from '../../actions/posts';

function Home() {
    const [currentId, setCurrentId] = useState(0);
    const [tags, setTags] = useState([]);
    const dispatch = useDispatch();
  
    useEffect(() => {
      dispatch(getPosts());
    }, [currentId, dispatch]);

    return (
        <Grow in>
        <Container>
          <Grid container justify="space-between" alignItems="stretch" spacing={3}>
            <Grid item xs={12} sm={7}>
              <Posts setCurrentId={setCurrentId} tags={tags} />
            </Grid>
            <Grid item xs={12} sm={4}>
              <Form currentId={currentId} setCurrentId={setCurrentId} setTags={setTags}/>
            </Grid>
          </Grid>
        </Container>
      </Grow>
    )
}

export default Home
