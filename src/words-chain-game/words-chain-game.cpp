#include <iostream>
#include <fstream>
#include <conio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <windows.h>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <atomic>
#include <random>

using namespace std;

// Constants
const int SCREEN_WIDTH = 120;
const int SCREEN_HEIGHT = 30;
const int TIMER_SECONDS = 10; // 10 seconds for word input

// Global variables
atomic<bool> timerExpired(false);
atomic<bool> timerRunning(false);
atomic<int> remainingTime(TIMER_SECONDS);
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO csbi;
HANDLE timerThreadHandle = NULL;
string currentUsername = "";
string player2Name = "";
int currentConsoleWidth = 120;

// Word dictionaries
vector<string> animals = {
    "aardvark", "albatross", "alligator", "alpaca", "anteater", "antelope", "armadillo", "baboon",
    "badger", "barracuda", "bat", "beaver", "bee", "bison", "boar", "buffalo", "butterfly",
    "camel", "capybara", "caribou", "cassowary", "cat", "caterpillar", "cattle", "chameleon",
    "cheetah", "chicken", "chimpanzee", "chinchilla", "chipmunk", "cobra", "cockroach", "cod",
    "coyote", "crab", "crane", "crocodile", "crow", "deer", "dingo", "dinosaur", "dog", "dolphin",
    "donkey", "dove", "dragonfly", "duck", "eagle", "echidna", "eel", "elephant", "elk", "emu",
    "falcon", "ferret", "finch", "fish", "flamingo", "fly", "fox", "frog", "gazelle", "gecko",
    "gerbil", "giraffe", "gnat", "gnu", "goat", "goldfish", "goose", "gorilla", "grasshopper",
    "grouse", "guineapig", "gull", "hamster", "hare", "hawk", "hedgehog", "heron", "herring",
    "hippopotamus", "hornet", "horse", "hummingbird", "hyena", "ibex", "ibis", "iguana", "impala",
    "jackal", "jaguar", "jay", "jellyfish", "kangaroo", "kingfisher", "kiwi", "koala", "komodo",
    "kookaburra", "lemur", "leopard", "lion", "llama", "lobster", "locust", "loris", "louse",
    "lynx", "lyrebird", "macaque", "macaw", "magpie", "mallard", "mammoth", "manatee", "mandrill",
    "manta", "marmoset", "marmot", "meerkat", "mink", "mole", "mongoose", "monkey", "moose",
    "mosquito", "mouse", "mule", "narwhal", "newt", "nightingale", "octopus", "okapi", "opossum",
    "orangutan", "oryx", "ostrich", "otter", "owl", "oyster", "panda", "panther", "parrot",
    "peacock", "pelican", "penguin", "pheasant", "pig", "pigeon", "piranha", "platypus", "pony",
    "porcupine", "porpoise", "possum", "prawn", "puffin", "puma", "quail", "quelea", "quetzal",
    "rabbit", "raccoon", "ram", "rat", "raven", "reindeer", "rhinoceros", "rook", "salamander",
    "salmon", "sandpiper", "sardine", "scorpion", "seahorse", "seal", "shark", "sheep", "shrew",
    "skunk", "sloth", "snail", "snake", "spider", "squid", "squirrel", "starfish", "stingray",
    "stoat", "stork", "swallow", "swan", "tapir", "tarsier", "termite", "tiger", "toad", "toucan",
    "trout", "turkey", "turtle", "viper", "vulture", "wallaby", "walrus", "wasp", "weasel",
    "whale", "wolf", "wombat", "woodpecker", "worm", "wren", "yak", "zebra"
};

vector<string> countries = {
    "afghanistan", "albania", "algeria", "andorra", "angola", "antigua", "argentina", "armenia",
    "australia", "austria", "azerbaijan", "bahamas", "bahrain", "bangladesh", "barbados", "belarus",
    "belgium", "belize", "benin", "bhutan", "bolivia", "bosnia", "botswana", "brazil", "brunei",
    "bulgaria", "burkina", "burundi", "cabo", "cambodia", "cameroon", "canada", "chad", "chile",
    "china", "colombia", "comoros", "congo", "costarica", "croatia", "cuba", "cyprus", "czechia",
    "denmark", "djibouti", "dominica", "ecuador", "egypt", "elsalvador", "eritrea", "estonia",
    "eswatini", "ethiopia", "fiji", "finland", "france", "gabon", "gambia", "georgia", "germany",
    "ghana", "greece", "grenada", "guatemala", "guinea", "guyana", "haiti", "honduras", "hungary",
    "iceland", "india", "indonesia", "iran", "iraq", "ireland", "israel", "italy", "jamaica",
    "japan", "jordan", "kazakhstan", "kenya", "kiribati", "kuwait", "kyrgyzstan", "laos", "latvia",
    "lebanon", "lesotho", "liberia", "libya", "liechtenstein", "lithuania", "luxembourg", "madagascar",
    "malawi", "malaysia", "maldives", "mali", "malta", "marshall", "mauritania", "mauritius",
    "mexico", "micronesia", "moldova", "monaco", "mongolia", "montenegro", "morocco", "mozambique",
    "myanmar", "namibia", "nauru", "nepal", "netherlands", "newzealand", "nicaragua", "niger",
    "nigeria", "northkorea", "northmacedonia", "norway", "oman", "pakistan", "palau", "palestine",
    "panama", "papua", "paraguay", "peru", "philippines", "poland", "portugal", "qatar", "romania",
    "russia", "rwanda", "saintkitts", "saintlucia", "samoa", "sanmarino", "saudiarabia", "senegal",
    "serbia", "seychelles", "sierraleone", "singapore", "slovakia", "slovenia", "solomon",
    "somalia", "southafrica", "southkorea", "southsudan", "spain", "srilanka", "sudan", "suriname",
    "sweden", "switzerland", "syria", "taiwan", "tajikistan", "tanzania", "thailand", "timor",
    "togo", "tonga", "trinidad", "tunisia", "turkey", "turkmenistan", "tuvalu", "uganda", "ukraine",
    "unitedarabemirates", "unitedkingdom", "unitedstates", "uruguay", "uzbekistan", "vanuatu",
    "vatican", "venezuela", "vietnam", "yemen", "zambia", "zimbabwe"
};

vector<string> fruits = {
    "apple", "apricot", "avocado", "banana", "bilberry", "blackberry", "blackcurrant", "blueberry",
    "boysenberry", "cantaloupe", "cherry", "clementine", "cloudberry", "coconut", "cranberry",
    "cucumber", "currant", "damson", "date", "dragonfruit", "durian", "elderberry", "fig",
    "gooseberry", "grape", "grapefruit", "guava", "honeydew", "huckleberry", "jackfruit", "jambul",
    "jujube", "kiwifruit", "kumquat", "lemon", "lime", "loquat", "longan", "lychee", "mandarin",
    "mango", "mangosteen", "marionberry", "melon", "mulberry", "nectarine", "nance", "olive",
    "orange", "papaya", "passionfruit", "peach", "pear", "persimmon", "physalis", "pineapple",
    "plum", "pomegranate", "pomelo", "quince", "raisin", "rambutan", "raspberry", "redcurrant",
    "salak", "satsuma", "starfruit", "strawberry", "tamarind", "tangelo", "tangerine", "tomato",
    "watermelon", "yuzu", "zucchini"
};

vector<string> professions = {
    "accountant", "actor", "actuary", "administrator", "advisor", "advocate", "agent", "analyst",
    "anesthesiologist", "animator", "anthropologist", "arbitrator", "archaeologist", "architect",
    "archivist", "artist", "astronaut", "astronomer", "attorney", "auditor", "author", "baker",
    "banker", "barber", "barista", "bartender", "biologist", "blacksmith", "blogger", "bookkeeper",
    "botanist", "brewer", "builder", "butcher", "cameraman", "captain", "cardiologist", "carpenter",
    "cartographer", "cashier", "chef", "chemist", "chiropractor", "cinematographer", "clerk",
    "coach", "cobbler", "comedian", "composer", "conductor", "consultant", "contractor", "cook",
    "cosmetologist", "counselor", "courier", "curator", "dancer", "dentist", "dermatologist",
    "designer", "detective", "developer", "diagnostician", "dietitian", "director", "doctor",
    "drafter", "driver", "economist", "editor", "educator", "electrician", "engineer", "entertainer",
    "entrepreneur", "epidemiologist", "estateagent", "executive", "farmer", "fashiondesigner",
    "filmmaker", "financialadvisor", "firefighter", "fisherman", "florist", "forester", "geologist",
    "graphicdesigner", "guard", "hairdresser", "historian", "host", "illustrator", "inspector",
    "instructor", "internist", "interpreter", "investigator", "journalist", "judge", "laborer",
    "landscaper", "lawyer", "librarian", "lifeguard", "linguist", "lobbyist", "locksmith", "logistician",
    "machinist", "manager", "manicurist", "marine", "marketer", "masseuse", "mathematician",
    "mechanic", "mediator", "merchant", "messenger", "midwife", "miller", "miner", "model",
    "musician", "nanny", "navigator", "negotiator", "neurologist", "nurse", "nutritionist",
    "obstetrician", "oceanographer", "oncologist", "operator", "ophthalmologist", "optometrist",
    "ornithologist", "orthodontist", "painter", "paramedic", "pathologist", "pediatrician",
    "performer", "pharmacist", "philanthropist", "philosopher", "photographer", "physician",
    "physicist", "pilot", "plumber", "podiatrist", "poet", "policeman", "politician", "professor",
    "programmer", "projectmanager", "psychiatrist", "psychologist", "publisher", "radiologist",
    "realtor", "receptionist", "recruiter", "referee", "reporter", "researcher", "restaurateur",
    "retailer", "salesperson", "scientist", "sculptor", "secretary", "securityguard", "senator",
    "singer", "socialworker", "sociologist", "soldier", "solicitor", "statistician", "steward",
    "stockbroker", "stylist", "surgeon", "surveyor", "tailor", "teacher", "technician", "therapist",
    "tourguide", "trader", "translator", "travelagent", "treasurer", "tutor", "umpire", "underwriter",
    "upholsterer", "veterinarian", "videographer", "waiter", "webdeveloper", "welder", "writer",
    "zoologist"
};

vector<string> games = {
    "amongus", "angrybirds", "animalcrossing", "apexlegends", "ark", "assassinscreed", "baldursgate",
    "batman", "battlefield", "bioshock", "borderlands", "callofduty", "candycrush", "civilization",
    "counterstrike", "crashbandicoot", "cryptokitties", "darkestdungeon", "destiny", "diablo",
    "dishonored", "doom", "dota", "dragonage", "dungeonsanddragons", "eveonline", "fallout",
    "farary", "farcry", "fifa", "finalfantasy", "fortnite", "godofwar", "grandtheftauto",
    "guitarhero", "half-life", "halo", "hearthstone", "hitman", "horizon", "injustice", "justcause",
    "killerinstinct", "kingdomhearts", "leagueoflegends", "left4dead", "legendofzelda", "limbo",
    "littlebigplanet", "luigismansion", "madden", "mario", "massiveeffect", "maxpayne", "metalgearsolid",
    "metroid", "minecraft", "monsterhunter", "mortalkombat", "needforspeed", "nintendogs", "overcooked",
    "overwatch", "pacman", "payday", "persona", "pokemon", "portal", "princeofpersia", "rayman",
    "reddeadredemption", "residentevil", "rocketleague", "sekiro", "shadowofcolossus", "shovelknight",
    "simcity", "sims", "skyrim", "sonic", "soulcalibur", "spider-man", "splatoon", "spyro",
    "starcraft", "streetfighter", "subnautica", "superhot", "supermeatboy", "supermariobros",
    "supermariosunshine", "supermarioworld", "teamfortress", "tetris", "thelastofus", "thesims",
    "thewitcher", "tombraider", "undertale", "uncharted", "valheim", "warcraft", "wario", "watchdogs",
    "worldofwarcraft", "xcom", "yoshi", "zelda"
};

vector<string> cities = {
    "abudhabi", "accra", "adelaide", "almaty", "amsterdam", "anchorage", "ankara", "asuncion",
    "athens", "atlanta", "auckland", "austin", "baghdad", "baku", "bangkok", "barcelona", "beijing",
    "beirut", "belgrade", "berlin", "bern", "birmingham", "bogota", "boston", "brazzaville",
    "brisbane", "brussels", "bucharest", "budapest", "buenosaires", "cairo", "calgary", "canberra",
    "cape", "caracas", "casablanca", "chandigarh", "charlotte", "chennai", "chicago", "chisinau",
    "christchurch", "colombo", "columbus", "copenhagen", "dakar", "dallas", "damascus", "darwin",
    "delhi", "denver", "detroit", "dhaka", "doha", "dubai", "dublin", "durban", "edinburgh",
    "edmonton", "frankfurt", "geneva", "glasgow", "gothenburg", "guangzhou", "guatemala", "halifax",
    "hamburg", "hanoi", "harare", "havana", "helsinki", "hochiminh", "hongkong", "honolulu",
    "houston", "hyderabad", "indianapolis", "istanbul", "jakarta", "jeddah", "jerusalem", "johannesburg",
    "kabul", "karachi", "kathmandu", "khartoum", "kingston", "kinshasa", "kolkata", "kualalumpur",
    "kuwait", "kyiv", "lagos", "lahore", "lasvegas", "lausanne", "leeds", "leipzig", "lima",
    "lisbon", "liverpool", "london", "losangeles", "luanda", "lublin", "luxembourg", "lyon",
    "madrid", "malaga", "managua", "manama", "manchester", "manila", "marseille", "medellin",
    "melbourne", "memphis", "mexico", "miami", "milan", "milwaukee", "minneapolis", "minsk",
    "montevideo", "montreal", "moscow", "mumbai", "munich", "muscat", "nagoya", "nairobi", "naples",
    "nashville", "newdelhi", "neworleans", "newyork", "nice", "nicosia", "niigata", "nuremberg",
    "odessa", "oklahoma", "omaha", "osaka", "oslo", "ottawa", "palermo", "panama", "paris",
    "perth", "philadelphia", "phnompenh", "phoenix", "pittsburgh", "portland", "porto", "prague",
    "pretoria", "providence", "quito", "raleigh", "reykjavik", "riga", "riodejaneiro", "riyadh",
    "rome", "rotterdam", "saintpetersburg", "saltlake", "salvador", "sanantonio", "sandiego",
    "sanfrancisco", "sanjose", "santiago", "saopaulo", "sapporo", "seattle", "seoul", "shanghai",
    "shenzhen", "singapore", "sofia", "stlouis", "stockholm", "stuttgart", "sydney", "taipei",
    "tallinn", "tampa", "tashkent", "tbilisi", "tehran", "telaviv", "thehague", "tijuana",
    "tokyo", "toronto", "toulouse", "tripoli", "tunis", "turku", "ulaanbaatar", "valencia",
    "vancouver", "venice", "vienna", "vientiane", "vilnius", "warsaw", "washington", "wellington",
    "winnipeg", "wroclaw", "yangon", "yerevan", "yokohama", "zagreb", "zurich"
};

vector<string> digitaldevices = {
    "airpods", "alexa", "android", "applewatch", "arduino", "barcode", "batterypack", "bluetooth",
    "calculator", "camera", "cassette", "charger", "chromebook", "clockradio", "computer", "controller",
    "cpu", "dashcam", "desktop", "digitalframe", "dongle", "drone", "dvdplayer", "earbuds",
    "echoshow", "electriccar", "electronicbook", "ethernet", "externalharddrive", "faxmachine",
    "fitnessband", "flashdrive", "flatscreen", "gameboy", "gamecube", "gamingconsole", "gaminglaptop",
    "gamingmouse", "gamingpc", "garmin", "googlehome", "gopro", "gps", "graphicstablet", "headphones",
    "headset", "homepod", "hometheater", "hoverboard", "hub", "inkjetprinter", "ipad", "iphone",
    "ipod", "joystick", "keyboard", "kindle", "laptop", "laserjet", "laserpointer", "laserprinter",
    "ledtv", "lens", "macbook", "macintosh", "mainframe", "microchip", "microphone", "microscope",
    "microwave", "minidisc", "minicomputer", "modem", "monitor", "motherboard", "mouse", "mp3player",
    "networkdrive", "networkprinter", "nintendo", "oculus", "oscilloscope", "pager", "palmtop",
    "pdas", "pendrive", "photocopier", "playstation", "plotter", "pointofsale", "powerbank",
    "printer", "projector", "radar", "radio", "raspberrypi", "recorder", "remotecontrol", "router",
    "samrtphone", "satellitedish", "scanner", "sd", "sega", "selfie", "server", "smartband",
    "smartbulb", "smartcar", "smartglasses", "smartlock", "smartmirror", "smartphone", "smartplug",
    "smartrefrigerator", "smartring", "smartscale", "smartshoes", "smartswitch", "smarttag",
    "smartthermostat", "smarttv", "smartwatch", "smartwaterbottle", "snes", "soundbar", "speaker",
    "ssd", "stylus", "supercomputer", "tablet", "telephone", "television", "thermostat", "touchpad",
    "touchscreen", "trackball", "trackpad", "transistor", "typewriter", "usb", "vcr", "videocamera",
    "videogame", "videophone", "virtualreality", "voicerecorder", "vrheadset", "walkman", "webcam",
    "wii", "windows", "workstation", "xbox", "zune"
};

vector<string> foods = {
    "adobo", "alfredo", "almond", "anchovy", "applepie", "apricotjam", "arancini", "arepa",
    "artichoke", "asparagus", "avocadotoast", "babaganoush", "bacon", "bagel", "bakedbeans",
    "baklava", "balsamic", "bamboo", "bananabread", "bangers", "barbecue", "basil", "bass",
    "beef", "beefstew", "beignet", "bento", "berries", "biryani", "biscuit", "bistec", "blackpudding",
    "blintz", "bluecheese", "bolognese", "bouillabaisse", "bread", "breadpudding", "brie",
    "broccoli", "brownie", "bruschetta", "bubbletea", "buffalowings", "bulgogi", "burrito",
    "butter", "butterchicken", "cabbage", "cake", "calamari", "calzone", "camembert", "candy",
    "cannoli", "cappuccino", "caprese", "caramel", "carbonara", "cardamom", "carrotcake", "cashew",
    "casserole", "cassoulet", "catfish", "cauliflower", "caviar", "ceviche", "challah", "champagne",
    "chana", "charcuterie", "cheddar", "cheese", "cheesecake", "cheesesteak", "cherry", "chicken",
    "chickennuggets", "chickenparm", "chickpea", "chili", "chimichanga", "chips", "chocolate",
    "chop", "chowder", "chowmein", "churro", "ciabatta", "cider", "cinnamon", "clafoutis",
    "clam", "clamchowder", "clams", "cobbler", "coconut", "cod", "coffee", "colacake", "cole",
    "compote", "cookie", "cordonbleu", "corn", "cornbread", "cornedbeef", "cornflakes", "couscous",
    "crab", "crabcake", "cranberry", "crayfish", "cream", "creamsoda", "cremebrulee", "crepe",
    "croissant", "crostini", "crouton", "crumble", "crumpet", "cucumber", "cupcake", "curd",
    "curry", "custard", "dahl", "dal", "dango", "dates", "deli", "dimsum", "dip", "dolma",
    "donut", "dosa", "doughnut", "dressing", "duck", "dumpling", "edamame", "egg", "eggplant",
    "eggnog", "eggs", "enchilada", "endive", "espresso", "fajita", "falafel", "farfalle", "farfalle",
    "fennel", "feta", "fettuccine", "fig", "filet", "fish", "fishandchips", "flan", "flatbread",
    "flautas", "focaccia", "fondue", "frankfurter", "frenchfries", "frenchtoast", "friedchicken",
    "friedrice", "frittata", "fritter", "froglegs", "frosting", "fruit", "fruitcake", "fudge",
    "galette", "garlic", "garlicbread", "gazpacho", "gelato", "ginger", "gingerbread", "gnocchi",
    "goat", "goatcheese", "goulash", "grahamcracker", "granola", "grape", "grapefruit", "gratin",
    "gravy", "greenbeans", "greens", "grenadine", "grilledcheese", "grits", "guacamole", "gumbo",
    "gyoza", "gyros", "haddock", "halibut", "halva", "ham", "hamburger", "harissa", "hash",
    "hawaiianpizza", "hazelnut", "herring", "hoagie", "hollandaise", "honey", "honeydew", "hotdog",
    "hotpot", "huevos", "hummus", "icecream", "icing", "injera", "insalata", "irishstew", "jambalaya",
    "jam", "jello", "jelly", "jerky", "jiaozi", "jicama", "kabob", "kale", "katsudon", "kebab",
    "kedgeree", "ketchup", "keylime", "kimchi", "kingcake", "kipper", "kitkat", "kohlrabi",
    "kombucha", "korma", "kugel", "kulfi", "labneh", "ladyfinger", "lamb", "lasagna", "latke",
    "lavender", "leek", "lemonade", "lentil", "lettuce", "lima", "lime", "linguine", "liver",
    "lobster", "loempia", "lollipop", "loukoumades", "macadamia", "macaroni", "macaroon", "mackerel",
    "madeline", "mahi", "mahimahi", "mai", "maitai", "mandarin", "mango", "manicotti", "maple",
    "marinara", "marmalade", "marshmallow", "martini", "masala", "mashedpotatoes", "matzo",
    "mayonnaise", "meatball", "meatloaf", "medialuna", "melon", "meringue", "milkshake", "mincemeat",
    "minestrone", "mint", "miso", "mojito", "molasses", "mole", "monkfish", "moose", "moussaka",
    "mozzarella", "muffin", "mugcake", "mukhwas", "mulligatawny", "mushroom", "mussels", "mustard",
    "nachos", "naan", "nachos", "nacho", "nectarine", "noodles", "nori", "nougat", "nutella",
    "oatmeal", "octopus", "okra", "olive", "omelet", "onion", "onionrings", "orange", "oregano",
    "oreo", "ossobuco", "oyster", "padthai", "paella", "palmier", "pancake", "pancetta", "paneer",
    "panini", "papaya", "paprika", "parfait", "parmesan", "parsley", "parsnip", "passionfruit",
    "pasta", "pastrami", "pastry", "pate", "patty", "pea", "peach", "peanut", "peanutbutter",
    "pear", "pecan", "pecorino", "pekingduck", "penne", "pepper", "pepperoni", "persimmon",
    "pho", "pickle", "pie", "pilaf", "pimento", "pineapple", "pinto", "pistachio", "pita",
    "pizza", "plantain", "plum", "pocky", "polenta", "pomegranate", "popcorn", "popover",
    "poppyseed", "popsicle", "pork", "porridge", "porterhouse", "portobello", "potato", "potatoes",
    "poutine", "prawn", "pretzel", "profiterole", "prosciutto", "provolone", "prune", "pudding",
    "pumpernickel", "pumpkin", "pumpkinpie", "quail", "quesadilla", "queso", "quiche", "quinoa",
    "rabbit", "raclette", "radicchio", "radish", "ragout", "raisin", "ramen", "ranch", "raspberry",
    "ratatouille", "ravioli", "redbeans", "redvelvet", "relish", "remoulade", "ribeye", "ribs",
    "rice", "ricecake", "ricotta", "risotto", "roastbeef", "rockcandy", "roe", "roll", "romaine",
    "rosemary", "rotini", "roulade", "rum", "rye", "saffron", "sage", "salad", "salami", "salmon",
    "salsa", "saltedcaramel", "samosa", "sandwich", "sangria", "sashimi", "sauerkraut", "sausage",
    "scallion", "scallop", "schnitzel", "scone", "seaweed", "sesame", "shallot", "sherbet",
    "shortbread", "shortcake", "shortrib", "shrimp", "sirloin", "skewer", "slaw", "smores",
    "snails", "snapper", "snowcone", "soda", "sole", "sorbet", "souffle", "soup", "sourdough",
    "soy", "soysauce", "spaghetti", "spanakopita", "spareribs", "spinach", "spritz", "sprouts",
    "squash", "squid", "steak", "stew", "stock", "strudel", "stuffing", "sturgeon", "submarine",
    "sugar", "sukiyaki", "sumac", "sundae", "sushi", "sweetbread", "sweetpotato", "swiss",
    "swordfish", "syrup", "taco", "tagine", "tahini", "tamale", "tamarind", "tandoori", "tangerine",
    "tapas", "tapioca", "taramasalata", "tart", "tartar", "tatertots", "tea", "tempeh", "tempura",
    "tequila", "teriyaki", "thyme", "tiramisu", "toast", "toffee", "tofu", "tomato", "torte",
    "tortellini", "tortilla", "trifle", "trout", "truffle", "tuna", "tunamelt", "turbot", "turkey",
    "turnip", "udon", "ugli", "unagi", "vanilla", "veal", "vegetable", "veloute", "venison",
    "vermouth", "vinegar", "vodka", "waffle", "walnut", "wasabi", "water", "watercress", "watermelon",
    "wedges", "wellington", "wheat", "whiskey", "whitechocolate", "whiting", "wine", "wings",
    "wonton", "wrap", "yakiniku", "yakitori", "yam", "yeast", "yellowtail", "yogurt", "yorkshire",
    "yuzu", "zabaglione", "ziti", "zucchini", "zuppa"
};

// Function prototypes
void gotoxy(int x, int y);
void centerText(const string& text, int y);
void centerTextWithOffset(const string& text, int y, int offset = 0);
void drawSimpleBox(int x1, int y1, int x2, int y2);
void printHeader();
void printLoginMenu();
void login();
void registerUser();
void showInstructions();
void showMainMenu();
void singlePlayerGame();
void twoPlayerGame();
void categorySelectionMenu(string& selectedCategory, vector<string>& selectedDictionary);
void startWordChainGame(bool isTwoPlayer, string category, vector<string> dictionary);
bool validateWord(const string& word, char lastChar, const vector<string>& dictionary, vector<string>& usedWords);
DWORD WINAPI timerThreadFunction(LPVOID lpParam);
void startTimer();
void stopTimer();
void resetTimer();
void updateTimerDisplay(int timeLeft, int width);
void clearScreenArea(int x1, int y1, int x2, int y2);
int getConsoleWidth();
int getConsoleHeight();
void getPlayer2Name();
void showHighScores();
void saveHighScore(string playerName, int score, string category);
string getInputAtPosition(int x, int y, int maxLength = 50);
void showCursor(bool visible);
string getAdvancedInput(int startX, int y, int fieldWidth, int maxLength, atomic<bool>& timerExpiredFlag);
int getCenteredX(int contentWidth);
int getMenuStartY(int itemCount);
int animatedMenu(const vector<string>& items, int highlightColor = 14, int normalColor = 7);

// Utility functions
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = static_cast<SHORT>(x);
    coord.Y = static_cast<SHORT>(y);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void showCursor(bool visible) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = visible;
    cursorInfo.dwSize = visible ? 25 : 1;
    SetConsoleCursorInfo(console, &cursorInfo);
}

int getConsoleWidth() {
    GetConsoleScreenBufferInfo(console, &csbi);
    currentConsoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return currentConsoleWidth;
}

int getConsoleHeight() {
    GetConsoleScreenBufferInfo(console, &csbi);
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

int getCenteredX(int contentWidth) {
    int width = getConsoleWidth();
    return (width - contentWidth) / 2;
}

void centerText(const string& text, int y) {
    int x = getCenteredX(text.length());
    if (x < 0) x = 0;
    gotoxy(x, y);
    cout << text;
}

void centerTextWithOffset(const string& text, int y, int offset) {
    int width = getConsoleWidth();
    size_t textLength = text.length();
    int x = static_cast<int>((width - static_cast<int>(textLength)) / 2) + offset;
    if (x < 0) x = 0;
    gotoxy(x, y);
    cout << text;
}

// Adaptive vertical layout
int getMenuStartY(int itemCount) {
    int height = getConsoleHeight();
    return (height - itemCount * 2) / 2;
}

// Animated menu system
int animatedMenu(const vector<string>& items, int highlightColor, int normalColor) {
    int selected = 0;
    int itemCount = items.size();
    int startY = getMenuStartY(itemCount);

    while (true) {
        for (int i = 0; i < itemCount; i++) {
            int x = getCenteredX(items[i].length() + 2); // +2 for the arrow
            int y = startY + i * 2;

            gotoxy(x - 2, y);

            if (i == selected) {
                SetConsoleTextAttribute(console, highlightColor);
                cout << "▶ " << items[i] << " ";
            }
            else {
                SetConsoleTextAttribute(console, normalColor);
                cout << "  " << items[i] << "  ";
            }
        }

        char key = _getch();

        if (key == 72) {             // ↑ arrow
            selected = (selected - 1 + itemCount) % itemCount;
            Beep(700, 20);
        }
        else if (key == 80) {        // ↓ arrow
            selected = (selected + 1) % itemCount;
            Beep(700, 20);
        }
        else if (key == '\r') {      // ENTER
            Beep(1000, 50);
            SetConsoleTextAttribute(console, 7);
            return selected;
        }

        Sleep(40); // smooth animation pacing
    }
}

void drawSimpleBox(int x1, int y1, int x2, int y2) {
    // Draw top border
    gotoxy(x1, y1);
    cout << "+";
    for (int i = x1 + 1; i < x2; i++) {
        cout << "-";
    }
    cout << "+";

    // Draw sides
    for (int i = y1 + 1; i < y2; i++) {
        gotoxy(x1, i); cout << "|";
        gotoxy(x2, i); cout << "|";
    }

    // Draw bottom border
    gotoxy(x1, y2);
    cout << "+";
    for (int i = x1 + 1; i < x2; i++) {
        cout << "-";
    }
    cout << "+";
}

void clearScreenArea(int x1, int y1, int x2, int y2) {
    for (int y = y1; y <= y2; y++) {
        gotoxy(x1, y);
        for (int x = x1; x <= x2; x++) {
            cout << " ";
        }
    }
}

// ADVANCED: Horizontal scrolling input with sound and visual indicators
string getAdvancedInput(
    int startX,
    int y,
    int fieldWidth,
    int maxLength,
    atomic<bool>& timerExpiredFlag
) {
    vector<char> buffer;
    int scrollOffset = 0;
    int cursor = 0;

    // Draw empty field
    for (int i = 0; i < fieldWidth; i++) {
        gotoxy(startX + i, y);
        cout << " ";
    }

    // Cursor config
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(console, &ci);
    ci.bVisible = true;
    ci.dwSize = 20;
    SetConsoleCursorInfo(console, &ci);

    while (!timerExpiredFlag) {
        if (_kbhit()) {
            char c = _getch();

            // ENTER
            if (c == '\r') break;

            // BACKSPACE
            if (c == '\b') {
                if (cursor > 0) {
                    cursor--;
                    buffer.erase(buffer.begin() + cursor);

                    if (scrollOffset > 0 && cursor < scrollOffset)
                        scrollOffset--;

                    Beep(600, 20); // sound
                }
            }
            // CHARACTER INPUT
            else if (isalpha(c) && buffer.size() < maxLength) {
                buffer.insert(buffer.begin() + cursor, tolower(c));
                cursor++;

                if (cursor - scrollOffset > fieldWidth - 1)
                    scrollOffset++;

                Beep(800, 15); // typing sound
            }

            // Redraw field (smooth overwrite)
            for (int i = 0; i < fieldWidth; i++) {
                gotoxy(startX + i, y);
                if (i + scrollOffset < buffer.size())
                    cout << buffer[i + scrollOffset];
                else
                    cout << " ";
            }

            // Cursor follows smoothly
            int cursorX = startX + (cursor - scrollOffset);
            if (cursorX < startX) cursorX = startX;
            if (cursorX > startX + fieldWidth - 1)
                cursorX = startX + fieldWidth - 1;

            gotoxy(cursorX, y);

            // Max-length indicator
            gotoxy(startX + fieldWidth + 2, y);
            SetConsoleTextAttribute(console,
                buffer.size() >= maxLength ? 12 : 8);
            cout << buffer.size() << "/" << maxLength;
            SetConsoleTextAttribute(console, 7);
        }
    }

    ci.bVisible = false;
    SetConsoleCursorInfo(console, &ci);

    return string(buffer.begin(), buffer.end());
}

// New function to get input at specific position with proper clearing
string getInputAtPosition(int x, int y, int maxLength) {
    showCursor(true);
    gotoxy(x, y);

    string input = "";
    char ch;

    while ((ch = static_cast<char>(_getch())) != '\r') {
        if (ch == '\b' && !input.empty()) {
            input.pop_back();
            gotoxy(x + input.length(), y);
            cout << " "; // Clear the character
            gotoxy(x + input.length(), y);
        }
        else if (ch != '\b' && isalpha(ch) && input.length() < maxLength) {
            input.push_back(tolower(ch));
            cout << ch;
        }
    }

    showCursor(false);
    return input;
}

void getPlayer2Name() {
    system("cls");
    printHeader();

    SetConsoleTextAttribute(console, 14);
    centerText("+---------------------------------------------+", 10);
    centerText("|           ENTER PLAYER 2 NAME               |", 11);
    centerText("+---------------------------------------------+", 12);
    SetConsoleTextAttribute(console, 7);

    centerText("Player 1: " + currentUsername, 15);

    int width = getConsoleWidth();

    // Prompt on line 17 - FIXED: Proper centering
    string prompt = "Enter Player 2 Name: ";
    int promptX = getCenteredX(prompt.length());
    gotoxy(promptX, 17);
    cout << prompt;

    // Clear area for input on line 18 (BELOW the prompt)
    int inputWidth = 30;
    int inputX = getCenteredX(inputWidth);
    gotoxy(inputX, 18);
    cout << "                      "; // Clear area

    // Get input on line 18
    player2Name = getInputAtPosition(inputX, 18, inputWidth);

    // If no name entered, use default
    if (player2Name.empty()) {
        player2Name = "Player 2";
    }

    centerText("Welcome " + player2Name + "!", 20);
    centerText("Press any key to continue...", 22);
    (void)_getch();
}

// Timer functions
DWORD WINAPI timerThreadFunction(LPVOID lpParam) {
    int width = *((int*)lpParam);
    remainingTime = TIMER_SECONDS;
    timerExpired = false;
    timerRunning = true;

    // Initial display
    updateTimerDisplay(remainingTime, width);

    while (remainingTime > 0 && timerRunning) {
        Sleep(1000); // Wait for 1 second

        if (timerRunning) {
            remainingTime--;
            updateTimerDisplay(remainingTime, width);
        }
    }

    if (remainingTime == 0 && timerRunning) {
        timerExpired = true;
        SetConsoleTextAttribute(console, 12); // Red
        int width = getConsoleWidth();
        gotoxy(width / 2 - 5, 21);
        cout << "TIME'S UP!";
        SetConsoleTextAttribute(console, 7); // Reset
    }

    timerRunning = false;
    return 0;
}

void startTimer(int width) {
    stopTimer(); // Stop any existing timer
    resetTimer();

    int* widthPtr = new int(width);
    timerThreadHandle = CreateThread(NULL, 0, timerThreadFunction, widthPtr, 0, NULL);
}

void stopTimer() {
    timerRunning = false;
    if (timerThreadHandle != NULL) {
        WaitForSingleObject(timerThreadHandle, 100);
        CloseHandle(timerThreadHandle);
        timerThreadHandle = NULL;
    }
}

void resetTimer() {
    stopTimer();
    timerExpired = false;
    remainingTime = TIMER_SECONDS;
}

void updateTimerDisplay(int timeLeft, int width) {
    if (timeLeft <= 3) {
        SetConsoleTextAttribute(console, 12); // Red color for low time
    }
    else if (timeLeft <= 5) {
        SetConsoleTextAttribute(console, 14); // Yellow color for medium time
    }
    else {
        SetConsoleTextAttribute(console, 10); // Green color for normal time
    }

    gotoxy(width - 20, 2);
    cout << "Time: " << setw(2) << setfill('0') << timeLeft << "s ";
    SetConsoleTextAttribute(console, 7); // Reset to white
}

// Game functions
void printHeader() {
    system("cls");
    SetConsoleTextAttribute(console, 13); // Magenta

    // Get console width for centering
    int width = getConsoleWidth();

    // Draw top border
    gotoxy(0, 0);
    for (int i = 0; i < width; i++) cout << "=";

    // Title
    centerText("================================================", 1);
    centerText("        W O R D   C H A I N   C H A L L E N G E ", 2);
    centerText("            ULTIMATE VOCABULARY SHOWDOWN        ", 3);
    centerText("================================================", 4);

    // Draw bottom border of header
    gotoxy(0, 5);
    for (int i = 0; i < width; i++) cout << "=";

    SetConsoleTextAttribute(console, 7); // Reset to white
}

void printLoginMenu() {
    printHeader();

    SetConsoleTextAttribute(console, 14);
    centerText("+---------------------------------------------+", 8);
    centerText("|           ACCOUNT AUTHENTICATION            |", 9);
    centerText("+---------------------------------------------+", 10);

    SetConsoleTextAttribute(console, 7);
}

void login() {
    int attempts = 3;
    while (attempts > 0) {
        printLoginMenu();

        string username, password, id, pass;
        bool found = false;

        // Username prompt - FIXED: Proper centering
        string prompt = "Enter Username: ";
        int promptX = getCenteredX(prompt.length());
        int inputY = 13;

        gotoxy(promptX, 12);
        cout << prompt;

        // input field width
        int inputWidth = 30;
        int inputX = getCenteredX(inputWidth);

        username = getInputAtPosition(inputX, inputY, inputWidth);

        // Password prompt - FIXED: Proper centering
        prompt = "Enter Password: ";
        promptX = getCenteredX(prompt.length());
        inputY = 15;

        gotoxy(promptX, 14);
        cout << prompt;

        inputX = getCenteredX(inputWidth);

        // Get password input on line 15
        gotoxy(inputX, inputY);
        password = "";
        char ch;
        showCursor(true);
        while ((ch = static_cast<char>(_getch())) != '\r') {
            if (ch == '\b' && !password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
            else if (ch != '\b') {
                password.push_back(ch);
                cout << "*";
            }
        }
        showCursor(false);

        // Check credentials
        ifstream loginFile("wordchain_login.txt");
        while (loginFile >> id >> pass) {
            if (id == username && pass == password) {
                found = true;
                break;
            }
        }
        loginFile.close();

        if (found) {
            currentUsername = username;
            centerText("Login Successful! Welcome " + username + "!", 17);
            SetConsoleTextAttribute(console, 10);
            centerText("Press any key to continue...", 19);
            SetConsoleTextAttribute(console, 7);
            (void)_getch();
            return;
        }
        else {
            attempts--;
            centerText("Invalid username or password! Attempts left: " + to_string(attempts), 17);
            if (attempts > 0) {
                centerText("Press any key to try again...", 19);
                (void)_getch();
            }
        }
    }

    centerText("Maximum attempts reached! Please register.", 19);
    centerText("Press any key to register...", 20);
    (void)_getch();
    registerUser();
}

void registerUser() {
    printHeader();

    SetConsoleTextAttribute(console, 14);
    centerText("+---------------------------------------------+", 8);
    centerText("|                 REGISTRATION                 |", 9);
    centerText("+---------------------------------------------+", 10);
    SetConsoleTextAttribute(console, 7);

    string username, password;
    bool usernameExists = false;

    do {
        usernameExists = false;

        // Username prompt - FIXED: Proper centering
        string prompt = "Choose Username: ";
        int promptX = getCenteredX(prompt.length());
        int inputY = 13;

        gotoxy(promptX, 12);
        cout << prompt;

        int inputWidth = 30;
        int inputX = getCenteredX(inputWidth);

        // Get username input on line 13
        username = getInputAtPosition(inputX, inputY, inputWidth);

        ifstream loginFile("wordchain_login.txt");
        string id, pass;
        while (loginFile >> id >> pass) {
            if (id == username) {
                usernameExists = true;
                centerText("Username already taken! Try another.", 15);
                Sleep(1000);

                // Clear the specific area
                int width = getConsoleWidth();
                clearScreenArea(width / 2 - 20, 12, width / 2 + 20, 16);
                break;
            }
        }
        loginFile.close();
    } while (usernameExists);

    // Password prompt - FIXED: Proper centering
    string prompt = "Choose Password: ";
    int promptX = getCenteredX(prompt.length());
    int inputY = 16;

    gotoxy(promptX, 15);
    cout << prompt;

    int inputWidth = 30;
    int inputX = getCenteredX(inputWidth);

    // Get password input on line 16
    gotoxy(inputX, inputY);
    password = "";
    char ch;
    showCursor(true);
    while ((ch = static_cast<char>(_getch())) != '\r') {
        if (ch == '\b' && !password.empty()) {
            password.pop_back();
            cout << "\b \b";
        }
        else if (ch != '\b') {
            password.push_back(ch);
            cout << "*";
        }
    }
    showCursor(false);

    // Save to file
    ofstream loginFile("wordchain_login.txt", ios::app);
    loginFile << username << " " << password << endl;
    loginFile.close();

    SetConsoleTextAttribute(console, 10);
    centerText("Registration Successful!", 18);
    centerText("Press any key to login...", 20);
    SetConsoleTextAttribute(console, 7);
    (void)_getch();

    login();
}

void showInstructions() {
    printHeader();

    SetConsoleTextAttribute(console, 14);
    centerText("+---------------------------------------------+", 7);
    centerText("|                 INSTRUCTIONS                 |", 8);
    centerText("+---------------------------------------------+", 9);
    SetConsoleTextAttribute(console, 7);

    centerText("================================================", 11);
    centerText("  HOW TO PLAY:                                 ", 12);
    centerText("  1. Choose game mode (Single or Two Player)   ", 13);
    centerText("  2. Select a word category                   ", 14);
    centerText("  3. Start with a given word                  ", 15);
    centerText("  4. Enter a word starting with last letter   ", 16);
    centerText("  5. Each player has 10 seconds per turn      ", 17);
    centerText("                                                ", 18);
    centerText("  RULES:                                       ", 19);
    centerText("  • Words must be from selected category       ", 20);
    centerText("  • No word repetitions allowed                ", 21);
    centerText("  • Each valid word = 10 points                ", 22);
    centerText("  • Invalid word = lose a life                 ", 23);
    centerText("  • Time's up = lose a life                   ", 24);
    centerText("  • Each player starts with 3 lives           ", 25);
    centerText("================================================", 26);

    SetConsoleTextAttribute(console, 14);
    centerText("Press any key to continue...", 30);
    SetConsoleTextAttribute(console, 7);
    (void)_getch();
}

void showMainMenu() {
    printHeader();

    SetConsoleTextAttribute(console, 14);
    centerText("+---------------------------------------------+", 10);
    centerText("|                MAIN MENU                    |", 11);
    centerText("+---------------------------------------------+", 12);
    SetConsoleTextAttribute(console, 7);

    // Use animated menu system
    vector<string> mainMenuItems = {
        "SINGLE PLAYER (vs Computer)",
        "TWO PLAYERS",
        "VIEW INSTRUCTIONS",
        "VIEW HIGH SCORES",
        "EXIT"
    };

    int choice = animatedMenu(mainMenuItems);

    // Clear the menu area before proceeding
    system("cls");

    switch (choice) {
    case 0:
        singlePlayerGame();
        break;
    case 1:
        twoPlayerGame();
        break;
    case 2:
        showInstructions();
        break;
    case 3:
        showHighScores();
        break;
    case 4:
        stopTimer();
        exit(0);
    }
}

void categorySelectionMenu(string& selectedCategory, vector<string>& selectedDictionary) {
    printHeader();

    SetConsoleTextAttribute(console, 14);
    centerText("+---------------------------------------------+", 8);
    centerText("|          SELECT WORD CATEGORY               |", 9);
    centerText("+---------------------------------------------+", 10);
    SetConsoleTextAttribute(console, 7);

    // Use animated menu system for category selection
    vector<string> categories = {
        "ANIMALS",
        "COUNTRIES",
        "FRUITS",
        "PROFESSIONS",
        "GAMES",
        "CITIES",
        "DIGITAL DEVICES",
        "FOODS",
        "MIXED"
    };

    int choice = animatedMenu(categories, 11);

    // Clear the menu area
    system("cls");
    printHeader();

    switch (choice) {
    case 0:
        selectedCategory = "ANIMALS";
        selectedDictionary = animals;
        break;
    case 1:
        selectedCategory = "COUNTRIES";
        selectedDictionary = countries;
        break;
    case 2:
        selectedCategory = "FRUITS";
        selectedDictionary = fruits;
        break;
    case 3:
        selectedCategory = "PROFESSIONS";
        selectedDictionary = professions;
        break;
    case 4:
        selectedCategory = "GAMES";
        selectedDictionary = games;
        break;
    case 5:
        selectedCategory = "CITIES";
        selectedDictionary = cities;
        break;
    case 6:
        selectedCategory = "DIGITAL DEVICES";
        selectedDictionary = digitaldevices;
        break;
    case 7:
        selectedCategory = "FOODS";
        selectedDictionary = foods;
        break;
    case 8:
        selectedCategory = "MIXED";
        selectedDictionary.clear();
        selectedDictionary.insert(selectedDictionary.end(), animals.begin(), animals.end());
        selectedDictionary.insert(selectedDictionary.end(), countries.begin(), countries.end());
        selectedDictionary.insert(selectedDictionary.end(), fruits.begin(), fruits.end());
        selectedDictionary.insert(selectedDictionary.end(), professions.begin(), professions.end());
        selectedDictionary.insert(selectedDictionary.end(), games.begin(), games.end());
        selectedDictionary.insert(selectedDictionary.end(), cities.begin(), cities.end());
        selectedDictionary.insert(selectedDictionary.end(), digitaldevices.begin(), digitaldevices.end());
        selectedDictionary.insert(selectedDictionary.end(), foods.begin(), foods.end());
        break;
    }

    // Show selected category
    centerText("Selected: " + selectedCategory, 15);
    centerText("Press any key to continue...", 17);
    (void)_getch();
}

bool validateWord(const string& word, char lastChar, const vector<string>& dictionary, vector<string>& usedWords) {
    // Check if word is not empty
    if (word.empty()) {
        return false;
    }

    // Check if word starts with the correct letter
    if (tolower(word[0]) != tolower(lastChar)) {
        return false;
    }

    // Check if word is in dictionary
    string lowerWord = word;
    transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);

    if (find(dictionary.begin(), dictionary.end(), lowerWord) == dictionary.end()) {
        return false;
    }

    // Check if word has already been used
    if (find(usedWords.begin(), usedWords.end(), lowerWord) != usedWords.end()) {
        return false;
    }

    return true;
}

void saveHighScore(string playerName, int score, string category) {
    ofstream highscore("wordchain_highscores.txt", ios::app);
    time_t now = time(nullptr);
    char buffer[26];
    ctime_s(buffer, sizeof(buffer), &now);
    highscore << playerName << " | Score: " << score << " | Category: " << category
        << " | Date: " << buffer;
    highscore.close();
}

void showHighScores() {
    system("cls");
    printHeader();
    SetConsoleTextAttribute(console, 14);
    centerText("HIGH SCORES", 10);
    SetConsoleTextAttribute(console, 7);

    ifstream highscore("wordchain_highscores.txt");
    if (highscore.is_open()) {
        string line;
        int y = 12;
        int count = 0;
        while (getline(highscore, line) && count < 10) {
            centerText(line, y++);
            count++;
        }
        highscore.close();

        if (count == 0) {
            centerText("No high scores yet! Be the first to set one!", 12);
        }
    }
    else {
        centerText("No high scores yet! Be the first to set one!", 12);
    }

    centerText("Press any key to continue...", 30);
    (void)_getch();
}

void startWordChainGame(bool isTwoPlayer, string category, vector<string> dictionary) {
    int player1Score = 0, player2Score = 0;
    int player1Lives = 3, player2Lives = 3;
    int round = 1;
    int maxRounds = 20;
    vector<string> usedWords;
    string currentWord;

    // Get starting word
    srand(static_cast<unsigned int>(time(nullptr)));
    int randomIndex = rand() % dictionary.size();
    currentWord = dictionary[randomIndex];
    usedWords.push_back(currentWord);

    string player1Name = currentUsername;
    string player2Name = isTwoPlayer ? ::player2Name : "COMPUTER";

    while (round <= maxRounds && (player1Lives > 0 && (isTwoPlayer ? player2Lives > 0 : true))) {
        system("cls");
        printHeader();

        int width = getConsoleWidth();

        // Game info display - responsive positioning
        SetConsoleTextAttribute(console, 11);
        centerTextWithOffset(player1Name + ": " + to_string(player1Score) + " pts | Lives: " + to_string(player1Lives), 7, -40);

        SetConsoleTextAttribute(console, isTwoPlayer ? 13 : 10);
        centerTextWithOffset(player2Name + ": " + to_string(player2Score) + " pts" + (isTwoPlayer ? " | Lives: " + to_string(player2Lives) : ""), 7, 40);

        SetConsoleTextAttribute(console, 14);
        centerText("ROUND " + to_string(round) + "/" + to_string(maxRounds) + " | Category: " + category, 7);
        SetConsoleTextAttribute(console, 7);

        // Current word display
        centerText("Current Word: " + currentWord, 10);

        // Last letter highlight
        char lastChar = currentWord.back();
        centerText("Next word must start with: [ " + string(1, char(toupper(lastChar))) + " ]", 12);

        // Player 1 turn
        centerText(player1Name + "'s Turn", 15);

        // Draw box
        int boxWidth = 50;
        int boxX = (width - boxWidth) / 2;
        drawSimpleBox(boxX, 17, boxX + boxWidth, 22);

        // Player 1 input area
        int promptX = boxX + 5;
        int promptY = 19;

        // Display prompt
        gotoxy(promptX, promptY);
        cout << "Enter word: ";

        // Fixed input field starts here
        int inputFieldX = promptX + 12; // length of "Enter word: "

        // Start timer for player 1
        startTimer(width);

        // Get ADVANCED overwrite-mode input with scrolling and sound
        string player1Word = getAdvancedInput(
            inputFieldX,
            promptY,
            25,        // visible width inside box
            40,        // REAL max length (scrolls after 25)
            timerExpired
        );

        stopTimer();

        // Handle player 1 input result
        gotoxy(boxX + 15, 21);
        if (timerExpired) {
            SetConsoleTextAttribute(console, 12);
            cout << "TIME'S UP!";
            SetConsoleTextAttribute(console, 7);
            player1Lives--;
            Sleep(1500);
        }
        else if (validateWord(player1Word, lastChar, dictionary, usedWords)) {
            SetConsoleTextAttribute(console, 10);
            cout << "VALID! +10 points";
            SetConsoleTextAttribute(console, 7);
            player1Score += 10;
            currentWord = player1Word;
            usedWords.push_back(player1Word);
            Sleep(1000);
        }
        else {
            SetConsoleTextAttribute(console, 12);
            cout << "INVALID! Lose a life";
            SetConsoleTextAttribute(console, 7);
            player1Lives--;
            Sleep(1500);
        }

        // If single player, computer plays
        if (!isTwoPlayer && player1Lives > 0 && round < maxRounds) {
            system("cls");
            printHeader();

            // Game info display
            SetConsoleTextAttribute(console, 11);
            centerTextWithOffset(player1Name + ": " + to_string(player1Score) + " pts | Lives: " + to_string(player1Lives), 7, -40);

            SetConsoleTextAttribute(console, 10);
            centerTextWithOffset(player2Name + ": " + to_string(player2Score) + " pts", 7, 40);

            SetConsoleTextAttribute(console, 14);
            centerText("ROUND " + to_string(round) + "/" + to_string(maxRounds) + " | Category: " + category, 7);
            SetConsoleTextAttribute(console, 7);

            // Current word display
            centerText("Current Word: " + currentWord, 10);

            // Last letter highlight
            lastChar = currentWord.back();
            centerText("Next word must start with: [ " + string(1, char(toupper(lastChar))) + " ]", 12);

            // Computer turn
            centerText("COMPUTER's Turn", 15);

            // Draw box
            drawSimpleBox(boxX, 17, boxX + boxWidth, 22);

            gotoxy(boxX + 5, 19);
            cout << "Computer is thinking...";

            // Computer finds a valid word
            string computerWord = "";
            for (const auto& word : dictionary) {
                if (validateWord(word, lastChar, dictionary, usedWords)) {
                    computerWord = word;
                    break;
                }
            }

            if (!computerWord.empty()) {
                gotoxy(boxX + 5, 20);
                cout << "Computer chose: " << computerWord;
                player2Score += 10;
                currentWord = computerWord;
                usedWords.push_back(computerWord);
                Sleep(2000);
            }
            else {
                gotoxy(boxX + 5, 20);
                cout << "Computer couldn't find a word!";
                Sleep(2000);
            }
        }
        // If two players, player 2 plays
        else if (isTwoPlayer && player1Lives > 0 && player2Lives > 0 && round < maxRounds) {
            // Clear screen for player 2 privacy
            system("cls");
            printHeader();
            centerText(player2Name + "'s turn (screen will clear for privacy)", 15);
            Sleep(2000);

            system("cls");
            printHeader();

            // Game info display
            SetConsoleTextAttribute(console, 13);
            centerText(player2Name + "'s Turn", 15);
            SetConsoleTextAttribute(console, 7);

            // Draw box
            drawSimpleBox(boxX, 17, boxX + boxWidth, 22);

            int player2PromptY = 19;

            gotoxy(promptX, player2PromptY);
            cout << "Current word ends with: " << char(toupper(lastChar));

            gotoxy(promptX, player2PromptY + 1);
            cout << "Enter word: ";

            // Fixed input field for player 2
            inputFieldX = promptX + 12;

            // Start timer for player 2
            resetTimer();
            startTimer(width);

            // Get ADVANCED overwrite-mode input with scrolling and sound
            string player2Word = getAdvancedInput(
                inputFieldX,
                player2PromptY + 1,
                25,        // visible width inside box
                40,        // REAL max length (scrolls after 25)
                timerExpired
            );

            stopTimer();

            // Handle player 2 input result
            gotoxy(boxX + 15, 21);
            if (timerExpired) {
                SetConsoleTextAttribute(console, 12);
                cout << "TIME'S UP!";
                SetConsoleTextAttribute(console, 7);
                player2Lives--;
                Sleep(1500);
            }
            else if (validateWord(player2Word, lastChar, dictionary, usedWords)) {
                SetConsoleTextAttribute(console, 10);
                cout << "VALID! +10 points";
                SetConsoleTextAttribute(console, 7);
                player2Score += 10;
                currentWord = player2Word;
                usedWords.push_back(player2Word);
                Sleep(1000);
            }
            else {
                SetConsoleTextAttribute(console, 12);
                cout << "INVALID! Lose a life";
                SetConsoleTextAttribute(console, 7);
                player2Lives--;
                Sleep(1500);
            }
        }

        round++;

        if (round <= maxRounds && player1Lives > 0 && (isTwoPlayer ? player2Lives > 0 : true)) {
            centerText("Next round in 3...", 25);
            Sleep(1000);
            centerText("Next round in 2...", 25);
            Sleep(1000);
            centerText("Next round in 1...", 25);
            Sleep(1000);
        }
    }

    stopTimer();

    // Determine winner
    system("cls");
    printHeader();

    int width = getConsoleWidth();

    SetConsoleTextAttribute(console, 14);
    centerText("+==========================================+", 10);
    centerText("|             GAME OVER!                   |", 11);
    centerText("+==========================================+", 12);

    SetConsoleTextAttribute(console, 7);

    // Display final scores
    SetConsoleTextAttribute(console, 11);
    centerText(player1Name + ": " + to_string(player1Score) + " points", 14);

    SetConsoleTextAttribute(console, isTwoPlayer ? 13 : 10);
    centerText(player2Name + ": " + to_string(player2Score) + " points", 15);

    // Save high score for single player
    if (!isTwoPlayer) {
        saveHighScore(currentUsername, player1Score, category);
    }

    // Determine winner
    SetConsoleTextAttribute(console, 14);
    if (player1Score > player2Score) {
        centerText(player1Name + " WINS THE GAME!", 17);
    }
    else if (player2Score > player1Score) {
        centerText(player2Name + " WINS THE GAME!", 17);
    }
    else {
        centerText("IT'S A TIE!", 17);
    }

    SetConsoleTextAttribute(console, 7);

    // Menu
    vector<string> gameOverMenu = {
        "Play Again",
        "Main Menu",
        "Exit"
    };

    // Game over menu
    int gameOverChoice = animatedMenu(gameOverMenu, 10, 7);

    switch (gameOverChoice) {
    case 0:
        if (isTwoPlayer) {
            twoPlayerGame();
        }
        else {
            singlePlayerGame();
        }
        break;
    case 1:
        return;
    case 2:
        exit(0);
    }
}

void singlePlayerGame() {
    string selectedCategory;
    vector<string> selectedDictionary;

    categorySelectionMenu(selectedCategory, selectedDictionary);
    startWordChainGame(false, selectedCategory, selectedDictionary);
}

void twoPlayerGame() {
    if (player2Name.empty()) {
        getPlayer2Name();
    }

    string selectedCategory;
    vector<string> selectedDictionary;

    categorySelectionMenu(selectedCategory, selectedDictionary);
    startWordChainGame(true, selectedCategory, selectedDictionary);
}

int main() {
    // Set console window properties
    system("title WORD CHAIN CHALLENGE - ULTIMATE VOCABULARY SHOWDOWN");

    // Set console to full screen mode
    system("mode con: cols=120 lines=40");

    // Change console font
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 8;
    cfi.dwFontSize.Y = 16;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(console, FALSE, &cfi);

    // Hide cursor initially (will show when needed)
    showCursor(false);

    // Login
    login();

    // Main game loop
    while (true) {
        showMainMenu();
    }

    return 0;
}