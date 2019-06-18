import React, { Component } from 'react';
import {
  StyleSheet,
  Text,
  View,
  Button,
  Alert,
  TouchableHighlight,
  Vibration,
  NativeModules
} from 'react-native';

import Swiper from 'react-native-swiper';
import Tts from 'react-native-tts';

const DURATION = 1000;

const styles = StyleSheet.create({
  wrapper: {
  },
  slide1: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#9DD6EB',
  },
  slide2: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#97CAE5',
  },
  slide3: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#92BBD9',
  },
  text: {
    color: '#fff',
    fontSize: 30,
    fontWeight: 'bold',
  },
  button: {
    width: 5000, 
    height: 5000,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#97CAE5'
  }
})

export default class Home extends Component {
  constructor(props) {
    super(props);
  }
  _onPressButton() {
    Alert.alert('Hello!');
    Vibration.vibrate(DURATION);
  }

  render(){
    return (
      <Swiper style={styles.wrapper} showsButtons={true}>
        <View style={styles.slide1}>
          <Text style={styles.text}>Hello Swiper</Text>
          <Button
            onPress={() => {
            Alert.alert('You tapped the button!');
            }}
            title="Press Me"
          />
        </View>
        <View style={styles.slide2}>
          <TouchableHighlight onPress={this._onPressButton} underlayColor="white">
            <View style={styles.button}>
              <Text style={styles.text}>Hello World!</Text>
            </View>
          </TouchableHighlight>
        </View>
        <View style={styles.slide3}>
          <Text style={styles.text}>And simple</Text>
        </View>
      </Swiper>
    );
  }
}