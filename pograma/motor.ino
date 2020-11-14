//class Motor {
//    int pointer = 0;
//    int pins [];
//
//    String sequence[8] = {"1100", "0100", "0110", "0010", "0011", "0001", "1001", "1000"};
//
//    void pinFunc(int vetor[], int range) {
//      for (int i = 0; i < range; i++) {
//        pinMode(vetor[i], OUTPUT);
//      }
//    }
//
//
//    public : Motor(int pins[]) {
//      pinFunc(pins, 4);
//      //this->pins = pins;
//    }
//
//    void setState(String input) {
//      Serial.println("setState");
//      if (input.charAt(0) == '1') {
//        digitalWrite(pins[0], 1);
//      } else {
//        digitalWrite(pins[0], 0);
//      }
//
//      if (input.charAt(1) == '1') {
//        digitalWrite(pins[1], 1);
//      } else {
//        digitalWrite(pins[1], 0);
//      }
//
//      if (input.charAt(2) == '1') {
//        digitalWrite(pins[2], 1);
//      } else {
//        digitalWrite(pins[2], 0);
//      }
//
//      if (input.charAt(3) == '1') {
//        digitalWrite(pins[3], 1);
//      } else {
//        digitalWrite(pins[3], 0);
//      }
//    }
//
//    public: void forward() {
//      setState(sequence[pointer]);
//      pointer = pointer + 1;
//      if (pointer == 8)pointer = 0;
//    }
//
//    public: void backward() {
//      setState(sequence[pointer]);
//      pointer = pointer - 1;
//      if (pointer == -1)pointer = 7;
//    }
//};
