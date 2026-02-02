import React from 'react';
import { fireEvent, render } from '@testing-library/react-native';
import App from '../App';

describe('App', () => {
  it('renders greeting copy', () => {
    const { getByTestId } = render(<App />);

    expect(getByTestId('headline-text')).toHaveTextContent('Hello from Cursor');
    expect(getByTestId('subtitle-text')).toHaveTextContent('TestFlight');
  });

  it('increments the tap counter when button is pressed', () => {
    const { getByTestId, getByText } = render(<App />);

    const button = getByTestId('cta-button');
    expect(getByText(/Tap me \(0\)/)).toBeTruthy();

    fireEvent.press(button);

    expect(getByText(/Tap me \(1\)/)).toBeTruthy();
  });
});
