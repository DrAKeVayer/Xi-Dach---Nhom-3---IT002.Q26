# Xi-Dach---Nhom-3---IT002.Q26

classDiagram
    class Hand {
        +vector~int~ hand
        +int score
        +bool isBust
        +bool hasAce
        +bool isSoft
        +bool isSplitable
        +calculateScore() void
        +receiveCard(int x) void
        +getHandType() HandType
        +getScore() int
        +printHand() void
    }

    class Player {
        +vector~Hand~ hands
        +resetPlayer() void
        +splitHand(int x) void
        +printHand() void
    }

    class Dealer {
        +printDealerFirstHand() void
        +printDealerHand() void
    }

    class Match {
        -vector~int~ deck
        -int cardIdx
        +vector~Player~ players
        +Dealer dealer
        +shuffle() void
        +drawCard() int
        +deal2Card() void
        +splitTo(Player& p, int x) void
    }

    class Stat {
        #int PWin
        #int DWin
        #int PBJWin
        ....
        +upPWin() void
        +upDWin() void
        +printStat() void
        ....
    }

    class HandType <<enumeration>> {
        THUONG
        NGULINH
        XIDACH
        XIBANG
    }

    class Action <<enumeration>> {
        HIT
        STAND
        SPLIT
        DOUBLED
    }

    Player "1" *-- "1..*" Hand : Composition (Has-a)
    Dealer --|> Player : Inheritance (Is-a)
    Match "1" o-- "1..*" Player : Aggregation
    Match "1" o-- "1" Dealer : Aggregation
    Match ..> Stat : Updates
    Hand ..> HandType : Returns
