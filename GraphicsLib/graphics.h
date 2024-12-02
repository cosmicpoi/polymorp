class GraphicsContext {
public:
    void initialize();  // Set up OpenGL or Metal
    void clearScreen(); // Clear the frame buffer
    void drawRectangle(const Vector2& position, const Vector2& size, const Color& color);
    void drawSprite(const Sprite& sprite, const Transform& transform);
    void present();     // Swap buffers
};