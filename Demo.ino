enum DemoPattern
{
    DEMO_CONFETTI,
    DEMO_JUGGLE,
    DEMO_CYLON,
    DEMO_BOUNCING_BALLS
};

DemoPattern currentDemoPattern = DEMO_CONFETTI;

uint32_t demoStartTime = 0;
const uint32_t DEMO_DURATION = 10000;   // 10 seconds each pattern


void demoSetup()
{
    currentDemoPattern = DEMO_CONFETTI;
    demoStartTime = millis();

    confettiSetup();
}

void demoLoop()
{
    if(millis() - demoStartTime > DEMO_DURATION)
    {
        demoStartTime = millis();

        switch(currentDemoPattern)
        {
            case DEMO_CONFETTI:
                currentDemoPattern = DEMO_CYLON;//DEMO_JUGGLE;
                juggle_setup();
                break;
            case DEMO_JUGGLE:
                currentDemoPattern = DEMO_CYLON;
                cylonSetup();
                break;
            case DEMO_CYLON:
                currentDemoPattern = DEMO_BOUNCING_BALLS;
                BouncingBallsSetup();
                break;
            case DEMO_BOUNCING_BALLS:
                currentDemoPattern = DEMO_CONFETTI;
                confettiSetup();
                break;                
        }
    }

    switch(currentDemoPattern)
    {
        case DEMO_CONFETTI:
            confettiStep();
            break;
        case DEMO_JUGGLE:
            juggleStep();
            break;
        case DEMO_CYLON:
            cylonStep();
            break;
        case DEMO_BOUNCING_BALLS:
            BouncingBallsStep();
            break;
    }
}