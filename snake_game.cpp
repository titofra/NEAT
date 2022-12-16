#include <iostream>
#include <cmath>
#include "./NEAT/population.cpp"

using namespace std;

float sigmoid(float x) {
    /* Will be our activation function */
    return 1 / (1 + exp(-1 * 4.09 * x));
}

struct args {
    /* global args for each genome */
    float score;
    int playgroundSize;
    int curMvmt;
    vector<int> snake;
    int fruit;
} snakeArgs;

bool isPosBusy(int pos, bool detectFruit, struct args* snakeArgs) {
    /* Check if something is at ths position */
    for (int i = 0; i < (int) (snakeArgs->snake.size()); i++) {
        if (snakeArgs->snake[i] == pos) {
            return true;
        }
    }
    return detectFruit && pos == snakeArgs->fruit;
}

void drawPlayground(struct args* snakeArgs) {
    /* Print the playground with circle for snake's part and sharp for fruits */
    for (int i = 0; i < snakeArgs->playgroundSize; i++) {
        for (int j = 0; j < snakeArgs->playgroundSize; j++) {
            if (isPosBusy(j + i * snakeArgs->playgroundSize, true, snakeArgs)) {
                if (isPosBusy(j + i * snakeArgs->playgroundSize, false, snakeArgs)) {
                    cout << " o ";
                } else {
                    cout << " # ";
                }
            } else {
                cout << " . ";
            }
        }
        cout << endl;
    }
}

int snakeEyes(int orientation, bool detectFruit, struct args* snakeArgs) {
    /* Return the number of empty case in the orientation relatively to playground (not movement!) */
    int curHeadPos = snakeArgs->snake[(int) snakeArgs->snake.size() - 1];
    int curXHeadPos = curHeadPos % snakeArgs->playgroundSize;
    int curYHeadPos = (int) (curHeadPos / snakeArgs->playgroundSize);
    int range;
    int orientationFactor;
    switch (orientation) {
        case 0: // ↖
            range =  min(curYHeadPos, curXHeadPos);
            orientationFactor = -1 * snakeArgs->playgroundSize - 1;
        break;
        case 1: // ↑
            range = curYHeadPos;
            orientationFactor = -1 * snakeArgs->playgroundSize;
        break;
        case 2: // ↗
            range = min(curYHeadPos, snakeArgs->playgroundSize - curXHeadPos - 1);
            orientationFactor = -1 * snakeArgs->playgroundSize + 1;
        break;
        case 3: // →
            range = snakeArgs->playgroundSize - curXHeadPos - 1;
            orientationFactor = 1;
        break;
        case 4: // ↘
            range = min(snakeArgs->playgroundSize - curYHeadPos - 1, snakeArgs->playgroundSize - curXHeadPos - 1);
            orientationFactor = snakeArgs->playgroundSize + 1;
        break;
        case 5: // ↓
            range = snakeArgs->playgroundSize - curYHeadPos - 1;
            orientationFactor = snakeArgs->playgroundSize;
        break;
        case 6: // ↙
            range = min(snakeArgs->playgroundSize - curYHeadPos - 1, curXHeadPos);
            orientationFactor = snakeArgs->playgroundSize - 1;
        break;
        case 7: // ←
            range = curXHeadPos;
            orientationFactor = - 1;
        break;
        default :
            cout << "Error: invalid orientation: " << orientation << endl;
            return -1;
        break;
    }
    int i = 1;
    while (i < range + 1 && !isPosBusy(curHeadPos + i * orientationFactor, detectFruit, snakeArgs)) {
        i ++;
    }
    i --;
    return i;
}

void setupSnake(float inputsInit[], struct args* snakeArgs) {
    /* initialise snakeArgs and first inputs */
    snakeArgs->playgroundSize = 8;
    snakeArgs->score = 0;
    snakeArgs->curMvmt = rand() % 4;
    snakeArgs->snake = {rand() % (snakeArgs->playgroundSize * snakeArgs->playgroundSize)};  // random position
    // choose fruit position on an empty place
    snakeArgs->fruit = rand() % (snakeArgs->playgroundSize * snakeArgs->playgroundSize);    // random position
    while (isPosBusy(snakeArgs->fruit, false, snakeArgs)) {
        snakeArgs->fruit = rand() % (snakeArgs->playgroundSize * snakeArgs->playgroundSize);    // random position
    }
    
    // playground bounds
    inputsInit[0] = snakeEyes((0 + 2 * snakeArgs->curMvmt) % 8, false, snakeArgs);
    inputsInit[1] = snakeEyes((1 + 2 * snakeArgs->curMvmt) % 8, false, snakeArgs);
    inputsInit[2] = snakeEyes((2 + 2 * snakeArgs->curMvmt) % 8, false, snakeArgs);
    inputsInit[3] = snakeEyes((3 + 2 * snakeArgs->curMvmt) % 8, false, snakeArgs);
    inputsInit[4] = snakeEyes((4 + 2 * snakeArgs->curMvmt) % 8, false, snakeArgs);
    inputsInit[5] = snakeEyes((5 + 2 * snakeArgs->curMvmt) % 8, false, snakeArgs);
    inputsInit[6] = snakeEyes((6 + 2 * snakeArgs->curMvmt) % 8, false, snakeArgs);
    
    // fruit
    inputsInit[7] = snakeEyes((0 + 2 * snakeArgs->curMvmt) % 8, true, snakeArgs);
    inputsInit[8] = snakeEyes((1 + 2 * snakeArgs->curMvmt) % 8, true, snakeArgs);
    inputsInit[9] = snakeEyes((2 + 2 * snakeArgs->curMvmt) % 8, true, snakeArgs);
    inputsInit[10] = snakeEyes((3 + 2 * snakeArgs->curMvmt) % 8, true, snakeArgs);
    inputsInit[11] = snakeEyes((4 + 2 * snakeArgs->curMvmt) % 8, true, snakeArgs);
    inputsInit[12] = snakeEyes((5 + 2 * snakeArgs->curMvmt) % 8, true, snakeArgs);
    inputsInit[13] = snakeEyes((6 + 2 * snakeArgs->curMvmt) % 8, true, snakeArgs);
}

float snakeProcess(float inputs[], float outputs[], struct args* snakeArgs) {
    /* The main function of the game: move the snake's part relatively to the inputs, refresh state, add fruits ... */
    // new Mvmt
    if (inputs[2] > 0.5 && inputs[2] > inputs[1] && inputs[2] > inputs[0]) {   // turn right
        snakeArgs->curMvmt = (snakeArgs->curMvmt + 1) % 4;
    } else {
        if (inputs[1] > 0.5 && inputs[1] > inputs[0] && inputs[1] > inputs[2]) {   // turn left
            snakeArgs->curMvmt = (snakeArgs->curMvmt - 1 + 4) % 4;  // + 4 to avoid having negative curMvmt
        } else {
            // consider inputs[0] dominent, is that a good idea ? idk
        }
    }
    
    // move snake
    int previousDot = snakeArgs->snake[(int) snakeArgs->snake.size() - 1];
    snakeArgs->snake.erase(snakeArgs->snake.begin() + 0);
    int newDot;
    switch (snakeArgs->curMvmt) {
        case 0:
            newDot = previousDot + 1;
        break;
        case 1:
            newDot = previousDot + snakeArgs->playgroundSize;
        break;
        case 2:
            newDot = previousDot - 1;
        break;
        case 3:
            newDot = previousDot - snakeArgs->playgroundSize;
        break;
        default :
            cout << "Error: invalid curMvmt: " << snakeArgs->curMvmt << endl;
            return -1;
        break;
    }
    
    // new snake's state
    if ((newDot % snakeArgs->playgroundSize != previousDot % snakeArgs->playgroundSize && (snakeArgs->curMvmt != 0 && snakeArgs->curMvmt != 2)) || ((int) (newDot / snakeArgs->playgroundSize) != (int) (previousDot / snakeArgs->playgroundSize) && (snakeArgs->curMvmt != 1 && snakeArgs->curMvmt != 3)) || (newDot < 0 || newDot >= snakeArgs->playgroundSize * snakeArgs->playgroundSize)) {   // the snake hit a bound
        return snakeArgs->score;   // return fitness
    }
    if (isPosBusy(newDot, true, snakeArgs)) {
        if (isPosBusy(newDot, false, snakeArgs)) { // the snake hit him
            return snakeArgs->score;   // return fitness
        } else {    // the snake eat the fruit
            snakeArgs->score += 100.0f;
            if ((int) snakeArgs->snake.size() == snakeArgs->playgroundSize * snakeArgs->playgroundSize - 1) {   // if the snake has the maximum lenght possible
                return snakeArgs->score;   // return fitness
            }
            // choose fruit position on an empty place
            snakeArgs->fruit = rand() % (snakeArgs->playgroundSize * snakeArgs->playgroundSize);    // random position
            while (isPosBusy(snakeArgs->fruit, false, snakeArgs)) {
                snakeArgs->fruit = rand() % (snakeArgs->playgroundSize * snakeArgs->playgroundSize);    // random position
            }
            snakeArgs->snake.insert(snakeArgs->snake.begin() + 0, snakeArgs->snake[0]);   // double the last dot too avoid to remove it the next frame
        }
    }
    
    snakeArgs->snake.push_back(newDot);
    // Get outputs. Those will be given to the network for the next generation
    // playground bounds
    outputs[0] = snakeEyes((0 + 2 * snakeArgs->curMvmt) % 8, false, snakeArgs);
    outputs[1] = snakeEyes((1 + 2 * snakeArgs->curMvmt) % 8, false, snakeArgs);
    outputs[2] = snakeEyes((2 + 2 * snakeArgs->curMvmt) % 8, false, snakeArgs);
    outputs[3] = snakeEyes((3 + 2 * snakeArgs->curMvmt) % 8, false, snakeArgs);
    outputs[4] = snakeEyes((4 + 2 * snakeArgs->curMvmt) % 8, false, snakeArgs);
    outputs[5] = snakeEyes((5 + 2 * snakeArgs->curMvmt) % 8, false, snakeArgs);
    outputs[6] = snakeEyes((6 + 2 * snakeArgs->curMvmt) % 8, false, snakeArgs);
    
    // fruit
    outputs[7] = snakeEyes((0 + 2 * snakeArgs->curMvmt) % 8, true, snakeArgs);
    outputs[8] = snakeEyes((1 + 2 * snakeArgs->curMvmt) % 8, true, snakeArgs);
    outputs[9] = snakeEyes((2 + 2 * snakeArgs->curMvmt) % 8, true, snakeArgs);
    outputs[10] = snakeEyes((3 + 2 * snakeArgs->curMvmt) % 8, true, snakeArgs);
    outputs[11] = snakeEyes((4 + 2 * snakeArgs->curMvmt) % 8, true, snakeArgs);
    outputs[12] = snakeEyes((5 + 2 * snakeArgs->curMvmt) % 8, true, snakeArgs);
    outputs[13] = snakeEyes((6 + 2 * snakeArgs->curMvmt) % 8, true, snakeArgs);
    
    snakeArgs->score += 1.0f;
    return -1.0f;  // game not finished
}

void playGame(Population* pop, int genomeId, int nbInput, int nbOutput, float activationFn(float input), int maxIterationsThresh) {
    /* Print a game played by the genome of id genomeId in population pop */
    args snakeArgs;
    
    // init inputs of snake game
    float procOutputs[nbInput];
    setupSnake(procOutputs, &snakeArgs);
    
    drawPlayground(&snakeArgs);
    
	float procInputs[nbOutput];
	float result = -1.0f;
	float iteration = 0;
	while (iteration < maxIterationsThresh && result < 0.0f) {
		pop->genomes[genomeId].loadInputs(procOutputs);
		pop->genomes[genomeId].runNetwork(activationFn);
		pop->genomes[genomeId].getOutputs(procInputs);
		
		result = snakeProcess(procInputs, procOutputs, &snakeArgs);
		
	    drawPlayground(&snakeArgs);
		
		iteration ++;
	}
	
	cout << "final score: " << result << endl;
}

int main() {
    srand(time(0));	// init seed for rand
    
    int popSize = 50;
    int nbInput = 14;
    int nbOutput = 3;
    int nbHiddenInit = 4;
    float probConnInit = 0.4f;
    bool areRecurrentConnectionsAllowed = false;
    float weightExtremumInit = 20.0f;
    float speciationThreshInit = 100.0f;
    int threshGensSinceImproved = 15;
    Population pop(popSize, nbInput, nbOutput, nbHiddenInit, probConnInit, areRecurrentConnectionsAllowed, weightExtremumInit, speciationThreshInit, threshGensSinceImproved);
    
    int target = 5;
    int targetThresh = 0;
    float stepThresh = 0.5f;
    float a = 1.0f;
    float b = 1.0f;
    float c = 0.4f;
    
    while (pop.generation < 10000) {
        pop.runNetworkAuto(snakeProcess, &snakeArgs, setupSnake, sigmoid, 500);
        pop.speciate(target, targetThresh, stepThresh, a, b, c);
        pop.crossover();
        pop.mutate();
    }
    
    // we have to run once again the network and to do a speciation to get the last fitter genome
    pop.runNetworkAuto(snakeProcess, &snakeArgs, setupSnake, sigmoid, 500);
    pop.speciate(target, targetThresh, stepThresh, a, b, c);
    
    // print information and draw the network of the fitter genome
    pop.printInfo(true, true, false, false);
    pop.drawNetwork(pop.fitterGenomeId);
    
    // play a game by the fitter genome
    playGame(&pop, pop.fitterGenomeId, nbInput, nbOutput, sigmoid, 500);
    
    return 0;
}
