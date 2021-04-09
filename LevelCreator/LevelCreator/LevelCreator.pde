
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

final int w = 32, h = 4, d = 32;
final float cellSize = 900 / (float)w;

ArrayList<byte[]> blocks;
ArrayList<Tool> tools;
Tool selectedTool;

int selectedx = -1, selectedy = 0, selectedz = -1;


void settings() {
  size((int)(cellSize * (w + 1)), (int)(cellSize * d));
}

void setup() {

  blocks = new ArrayList<byte[]>();

  for (int i = 0; i < h; i++) {
    blocks.add(new byte[w * d]);

    for (int j = 0; j < d; j++) {
      for (int k = 0; k < w; k++) {
        blocks.get(i)[j * w + k] = 0;
      }
    }
  }

  for (int j = 0; j < d; j++) {
    for (int k = 0; k < w; k++) {
      blocks.get(0)[j * w + k] = 1;
    }
  }

  tools = new ArrayList<Tool>();

  tools.add(new SetBlockTool((byte)0));
  tools.add(new SetBlockTool((byte)1));
}


color blockColor(byte block) {
  switch(block) {
  case 1:
    return color(255);

  default:
    return color(0);
  }
}

void draw() {

  background(255);

  for (int j = 0; j < d; j++) {
    for (int k = 0; k < w; k++) {
      byte block = blocks.get(selectedy)[j * w + k];
      fill(blockColor(block));
      stroke(0, 0);
      rect(k * cellSize, j * cellSize, cellSize, cellSize);
    }
  }

  for (int i = 0; i < tools.size(); i++) {
    tools.get(i).draw(w, i);
  }

  stroke(0);
  strokeWeight(1);
  for (int i = 0; i <= w; i++) {
    line(i * cellSize, 0, i * cellSize, cellSize * d);
  }
  for (int i = 0; i <= d; i++) {
    line(0, i * cellSize, cellSize * w, i * cellSize);
  }

  if (selectedx != -1) {
    stroke(255, 0, 0, 255);
    strokeWeight(3);
    fill(0, 0);
    rect(selectedx * cellSize, selectedz * cellSize, cellSize * ((int)(mouseX / cellSize) - selectedx + 1), cellSize * ((int)(mouseY / cellSize) - selectedz + 1));
  }

  if (selectedTool != null) {
    stroke(0, 255, 0, 255);
    strokeWeight(3);
    fill(0, 0);
    rect(w * cellSize, tools.indexOf(selectedTool) * cellSize, cellSize, cellSize);
  }
}

void keyPressed(KeyEvent e) {
  if (selectedx == -1) return;
}


void selectTool(int x, int y) {
  if (y >= tools.size()) return;

  if (tools.indexOf(selectedTool) == y) {
    selectedTool = null;
    return;
  }

  selectedTool = tools.get(y);
}

void mousePressed(MouseEvent e) {
  int x = (int)(e.getX() / cellSize), y = (int)(e.getY() / cellSize);

  if (x >= w) {
    return;
  }

  selectedx = x;
  selectedz = y;
}

void mouseReleased(MouseEvent e) {
  int x = (int)(e.getX() / cellSize), y = (int)(e.getY() / cellSize);

  if (x >= w) {
    selectTool(x, y);
    return;
  }

  if (selectedTool != null) {
    for (int i = selectedx; i <= x && i < w; i++) {
      for (int j = selectedz; j <= y && j < d; j++) {
        selectedTool.use(i, j);
      }
    }
  }


  selectedx = -1;
  selectedz = -1;
}

void mouseWheel(MouseEvent e) {
  selectedy -= e.getCount();
  selectedy = constrain(selectedy, 0, h - 1);
}

void exit() {
  ByteBuffer bb = ByteBuffer.allocate(12 + w * h * d);
  bb.order(ByteOrder.LITTLE_ENDIAN);
  bb.putInt(w);
  bb.putInt(h);
  bb.putInt(d);
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < d; j++) {
      for (int k = 0; k < w; k++) {
        byte block = blocks.get(i)[j * w + k];
        bb.put(block);
      }
    }
  }
  
  
  saveBytes("level.bin", bb.array());
}
