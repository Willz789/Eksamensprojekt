abstract class Tool {  
  
  abstract void use(int x, int y);
  abstract void draw(int x, int y);
 
}

class SetBlockTool extends Tool {
  
  public byte blockId;
  
  public SetBlockTool(byte blockId) {
    this.blockId = blockId;
  }
  
  @Override
  void use(int x, int y) {
    blocks.get(selectedy)[y * d + x] = blockId;
  }
  
  @Override
  void draw(int x, int y) {
      fill(blockColor(blockId));
      stroke(0, 0);
      rect(x * cellSize, y * cellSize, cellSize, cellSize);
  }
  
}
