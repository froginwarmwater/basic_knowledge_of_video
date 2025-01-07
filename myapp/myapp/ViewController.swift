//
//  ViewController.swift
//  myapp
//
//  Created by 李昕阳 on 31/12/2024.
//

import Cocoa

class ViewController: NSViewController {
    
    var recStatus: Bool = false
    var thread: Thread?
    let btn = NSButton.init(title: "Button", target: nil, action: nil)
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        self.view.setFrameSize(NSSize(width: 320, height: 240))
        
        
        btn.title = "start"
        btn.frame = NSRect(x: 320 / 2 - 40, y: 240 / 2 - 15, width: 80, height: 30)
        btn.setButtonType(.pushOnPushOff)
        btn.bezelStyle = .rounded
        
        // callback
        btn.target = self
        btn.action = #selector(myfunc)
        self.view.addSubview(btn)
    }
    
    @objc
    func myfunc() {
        self.recStatus = !self.recStatus
        
        if recStatus {
            self.thread = Thread.init(target: self, selector: #selector(self.recAudio), object: nil)
            self.thread?.start()
            self.btn.title = "stop"
        } else {
            set_rec_status(0)
            self.btn.title = "start"
            
        }
//       record_audio()
    }
    @objc
    func recAudio() {
//        print("start thread")
        record_audio()
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }


}

