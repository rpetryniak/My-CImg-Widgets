class MyApplicationParameters {
  private:
    int activeWindowId;
  public:
    void setActiveWindowId(int window_id) { activeWindowId = window_id; }
    int  getActiveWindowId() { return activeWindowId; }
};
