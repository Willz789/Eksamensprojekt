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
    if (blockId%64 != 5) {
      blocks.get(selectedy)[y * d + x] = blockId;
    } else {
      if (blocks.get(selectedy)[y * d + x] >= 192) {
        blocks.get(selectedy)[y * d + x] -= 192;
      } else {
        blocks.get(selectedy)[y * d + x] += 64;
        
      }
      println(blocks.get(selectedy)[y * d + x] & 0xff);
    }
  }

  @Override
  void draw(int x, int y) {
    fill(blockColor(blockId));
    stroke(0, 0);
    rect(x * cellSize, y * cellSize, cellSize, cellSize);
    if (blockId == 5) { // Hvis selectedTool er "Rotate"
      fill(50);
      textSize(15);
      textAlign(CENTER);
      pushMatrix();
      text("R", x*cellSize+cellSize/2, y*cellSize+cellSize/2);
      popMatrix();
    }
  }
}
