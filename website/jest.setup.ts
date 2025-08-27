import "@testing-library/jest-dom";

/* matchMedia (pour Next/Tailwind) */
Object.defineProperty(window, "matchMedia", {
  writable: true,
  value: (query: string) => ({
    matches: false,
    media: query,
    onchange: null,
    addListener: () => {}, // deprecated
    removeListener: () => {}, // deprecated
    addEventListener: () => {},
    removeEventListener: () => {},
    dispatchEvent: () => false,
  }),
});

/* ResizeObserver (utilisé par le composant) */
class ResizeObserverMock {
  observe(_target?: Element): void {}
  unobserve(_target?: Element): void {}
  disconnect(): void {}
}
Object.defineProperty(globalThis, "ResizeObserver", {
  writable: true,
  value: ResizeObserverMock,
});

/* Image.decode pour <img> */
Object.defineProperty(global.Image.prototype, "decode", {
  configurable: true,
  value: () => Promise.resolve(),
});

/* requestAnimationFrame / cancelAnimationFrame (anim légères) */
Object.defineProperty(window, "requestAnimationFrame", {
  writable: true,
  value: (cb: FrameRequestCallback) => setTimeout(() => cb(performance.now()), 16),
});
Object.defineProperty(window, "cancelAnimationFrame", {
  writable: true,
  value: (id: number) => clearTimeout(id),
});

/* Viewport */
Object.defineProperty(window, "innerWidth", { value: 1200, writable: true });
Object.defineProperty(window, "innerHeight", { value: 800, writable: true });

/* Cleanup global minimal */
afterEach(() => {
  localStorage.clear();
  jest.restoreAllMocks();
});
