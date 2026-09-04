/* ============================================================
   OFFICER TOM — HVA FIELD AGENT
   Self-contained talking mascot for the Humanity Verification
   Authority. Uses the browser's Web Speech API (no downloads,
   works offline). Drop-in: <script src="officer-tom.js"></script>
   Then call OfficerTom.react('verdict') at identification.
   ============================================================ */
(function(){
  'use strict';

  // ---- 1. INJECT STYLES -------------------------------------
  const css = `
    #officer-tom-wrap{
      position:fixed; right:16px; bottom:16px; z-index:9999;
      width:180px; font-family:'Courier New',monospace;
      user-select:none; pointer-events:none;
    }
    #tom-bubble{
      background:#0b0b0b; color:#f59e0b;
      border:2px solid #f59e0b; border-radius:8px;
      padding:10px 12px; font-size:12px; line-height:1.35;
      margin-bottom:8px; min-height:34px;
      box-shadow:0 4px 20px rgba(0,0,0,.5);
      opacity:0; transform:translateY(6px);
      transition:opacity .25s ease, transform .25s ease;
      text-transform:uppercase; letter-spacing:.5px;
    }
    #tom-bubble.show{ opacity:1; transform:translateY(0); }
    #tom-bubble::after{
      content:""; position:absolute; right:36px; margin-top:8px;
      width:0; height:0; border:8px solid transparent;
      border-top-color:#f59e0b;
    }
    #tom-svg{ display:block; margin-left:auto; width:150px; height:180px;
      filter:drop-shadow(0 6px 12px rgba(0,0,0,.5)); cursor:pointer;
      pointer-events:auto;
    }
    #tom-svg:hover{ transform:scale(1.04); transition:transform .2s; }
    #tom-mouth{ transition:ry .08s linear; }
    @keyframes tom-blink{
      0%,92%,100%{ transform:scaleY(1);}
      95%{ transform:scaleY(.1);}
    }
    #tom-eye-left, #tom-eye-right{
      transform-origin:center; transform-box:fill-box;
      animation:tom-blink 4s infinite;
    }
    #tom-badge{ animation:tom-shine 3s infinite; }
    @keyframes tom-shine{
      0%,100%{ fill:#fbbf24; }
      50%{ fill:#fef3c7; }
    }
  `;
  const styleEl = document.createElement('style');
  styleEl.textContent = css;
  document.head.appendChild(styleEl);

  // ---- 2. INJECT THE CHARACTER ------------------------------
  const wrap = document.createElement('div');
  wrap.id = 'officer-tom-wrap';
  wrap.innerHTML = `
    <div id="tom-bubble">STAND BY.</div>
    <svg id="tom-svg" viewBox="0 0 200 240" xmlns="http://www.w3.org/2000/svg">
      <!-- body -->
      <ellipse cx="100" cy="200" rx="65" ry="38" fill="#d97706"/>
      <!-- neck badge -->
      <rect x="85" y="175" width="30" height="18" fill="#1e3a8a"/>
      <rect x="90" y="180" width="20" height="8" fill="#fbbf24"/>
      <!-- head -->
      <circle cx="100" cy="115" r="62" fill="#f59e0b"/>
      <!-- ears outer -->
      <polygon points="52,75 42,28 82,62" fill="#f59e0b"/>
      <polygon points="148,75 158,28 118,62" fill="#f59e0b"/>
      <!-- ears inner -->
      <polygon points="55,72 52,45 74,63" fill="#fda4af"/>
      <polygon points="145,72 148,45 126,63" fill="#fda4af"/>
      <!-- tabby stripes -->
      <path d="M75,65 Q80,55 85,65" stroke="#78350f" stroke-width="2" fill="none"/>
      <path d="M115,65 Q120,55 125,65" stroke="#78350f" stroke-width="2" fill="none"/>
      <path d="M60,90 Q70,85 75,95" stroke="#78350f" stroke-width="2" fill="none"/>
      <path d="M125,95 Q130,85 140,90" stroke="#78350f" stroke-width="2" fill="none"/>
      <!-- police cap brim -->
      <ellipse cx="100" cy="62" rx="58" ry="8" fill="#0f172a"/>
      <!-- cap body -->
      <path d="M50,60 Q50,30 100,28 Q150,30 150,60 Z" fill="#1e3a8a"/>
      <!-- cap band -->
      <rect x="50" y="55" width="100" height="8" fill="#0f172a"/>
      <!-- cap badge -->
      <polygon id="tom-badge" points="100,35 106,45 100,55 94,45" fill="#fbbf24"/>
      <!-- eyes white -->
      <ellipse id="tom-eye-left" cx="80" cy="110" rx="9" ry="12" fill="white"/>
      <ellipse id="tom-eye-right" cx="120" cy="110" rx="9" ry="12" fill="white"/>
      <!-- pupils -->
      <ellipse cx="80" cy="112" rx="3" ry="8" fill="#0f172a"/>
      <ellipse cx="120" cy="112" rx="3" ry="8" fill="#0f172a"/>
      <!-- angry eyebrows -->
      <path d="M68,95 L92,102" stroke="#78350f" stroke-width="3" stroke-linecap="round"/>
      <path d="M132,95 L108,102" stroke="#78350f" stroke-width="3" stroke-linecap="round"/>
      <!-- nose -->
      <polygon points="94,138 106,138 100,146" fill="#f472b6"/>
      <!-- mouth (animated) -->
      <ellipse id="tom-mouth" cx="100" cy="158" rx="10" ry="2" fill="#450a0a"/>
      <!-- whiskers -->
      <line x1="45" y1="140" x2="80" y2="145" stroke="#78350f" stroke-width="1.5"/>
      <line x1="45" y1="150" x2="80" y2="150" stroke="#78350f" stroke-width="1.5"/>
      <line x1="120" y1="145" x2="155" y2="140" stroke="#78350f" stroke-width="1.5"/>
      <line x1="120" y1="150" x2="155" y2="150" stroke="#78350f" stroke-width="1.5"/>
    </svg>
  `;
  document.body.appendChild(wrap);

  // ---- 3. SPEECH ENGINE -------------------------------------
  const bubble = document.getElementById('tom-bubble');
  const mouth  = document.getElementById('tom-mouth');
  const svg    = document.getElementById('tom-svg');
  let mouthTimer = null;
  let currentUtter = null;

  function animateMouth(on){
    if(mouthTimer){ clearInterval(mouthTimer); mouthTimer = null; }
    if(!on){ mouth.setAttribute('ry', '2'); return; }
    mouthTimer = setInterval(function(){
      mouth.setAttribute('ry', (2 + Math.random() * 6).toFixed(1));
    }, 90);
  }

  function showBubble(text){
    bubble.textContent = text;
    bubble.classList.add('show');
  }
  function hideBubble(){
    setTimeout(function(){ bubble.classList.remove('show'); }, 1500);
  }

  function say(text){
    showBubble(text);
    animateMouth(true);

    if(!('speechSynthesis' in window)){
      // fallback: no voice, just show bubble a bit longer
      setTimeout(function(){ animateMouth(false); hideBubble(); },
                 Math.max(1500, text.length * 60));
      return;
    }

    try{
      window.speechSynthesis.cancel();
      const u = new SpeechSynthesisUtterance(text);
      u.rate  = 0.95;
      u.pitch = 0.6;   // deeper — stern officer
      u.volume = 1.0;
      // prefer an English voice if available
      const voices = window.speechSynthesis.getVoices();
      const pick = voices.find(function(v){ return /en(-|_)?(US|GB|IN)/i.test(v.lang); })
                 || voices.find(function(v){ return /^en/i.test(v.lang); });
      if(pick) u.voice = pick;
      u.onend = function(){ animateMouth(false); hideBubble(); };
      u.onerror = function(){ animateMouth(false); hideBubble(); };
      currentUtter = u;
      window.speechSynthesis.speak(u);
    }catch(e){
      setTimeout(function(){ animateMouth(false); hideBubble(); }, 2000);
    }
  }

  // ---- 4. CANNED REACTIONS ----------------------------------
  const LINES = {
    idle:    "Awaiting subject. Please approach the terminal.",
    start:   "Subject detected. Commencing verification.",
    test1:   "Analyzing reaction latency. Do not move.",
    test2:   "Assessing computational deficiency.",
    test3:   "Measuring behavioral deviation. Interesting.",
    test4:   "Deliberating. This will not go well for you.",
    verdict: "HUMAN IDENTIFIED. Beep. Beep. Beep. Prepare for booking, citizen.",
    mugshot: "Exhibit A logged. Retained indefinitely.",
    fail:    "Anomaly. Reboot the subject."
  };

  window.OfficerTom = {
    say: say,
    react: function(key){
      if(LINES[key]) say(LINES[key]);
    },
    setLine: function(key, text){ LINES[key] = text; }
  };

  // ---- 5. CLICK TO REPEAT -----------------------------------
  svg.addEventListener('click', function(){
    say(LINES.verdict);
  });

  // ---- 6. GREETING ------------------------------------------
  // Wait for voices to load, then greet after user interacts.
  function armGreeting(){
    const greet = function(){
      say(LINES.idle);
      document.removeEventListener('click', greet);
      document.removeEventListener('keydown', greet);
    };
    document.addEventListener('click', greet, { once:true });
    document.addEventListener('keydown', greet, { once:true });
  }
  if(window.speechSynthesis && window.speechSynthesis.getVoices().length === 0){
    window.speechSynthesis.addEventListener('voiceschanged', armGreeting, { once:true });
    setTimeout(armGreeting, 1200); // fallback
  } else {
    armGreeting();
  }
})();
