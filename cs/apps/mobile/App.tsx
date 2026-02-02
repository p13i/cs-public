import React, { useState } from 'react';
import { SafeAreaView, StatusBar, StyleSheet, Text, View, Pressable } from 'react-native';
import { StatusBar as ExpoStatusBar } from 'expo-status-bar';

export default function App(): JSX.Element {
  const [tapCount, setTapCount] = useState<number>(0);

  const handlePress = () => {
    setTapCount((count) => count + 1);
  };

  return (
    <SafeAreaView style={styles.safeArea}>
      <StatusBar barStyle="dark-content" />
      <ExpoStatusBar style="auto" />
      <View style={styles.container}>
        <Text style={styles.headline} testID="headline-text">
          Hello from Cursor 👋
        </Text>
        <Text style={styles.subtitle} testID="subtitle-text">
          This Expo-managed React Native app is ready for TestFlight.
        </Text>
        <Pressable
          accessibilityRole="button"
          style={({ pressed }) => [styles.button, pressed && styles.buttonPressed]}
          onPress={handlePress}
          testID="cta-button"
        >
          <Text style={styles.buttonLabel}>Tap me ({tapCount})</Text>
        </Pressable>
      </View>
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  safeArea: {
    flex: 1,
    backgroundColor: '#f7f7f7'
  },
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
    padding: 24
  },
  headline: {
    fontSize: 28,
    fontWeight: '600',
    marginBottom: 12
  },
  subtitle: {
    fontSize: 16,
    textAlign: 'center',
    color: '#555',
    marginBottom: 24
  },
  button: {
    backgroundColor: '#2e6ef7',
    borderRadius: 8,
    paddingHorizontal: 24,
    paddingVertical: 12
  },
  buttonPressed: {
    opacity: 0.85
  },
  buttonLabel: {
    color: '#fff',
    fontSize: 16,
    fontWeight: '600'
  }
});
