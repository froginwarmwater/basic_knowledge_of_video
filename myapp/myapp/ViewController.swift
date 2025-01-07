//
//  ViewController.swift
//  myapp
//
//  Created by 李昕阳 on 31/12/2024.
//

import Cocoa

class ViewController: NSViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        self.view.setFrameSize(NSSize(width: 320, height: 240))
        
        let btn = NSButton.init(title: "Button", target: nil, action: nil)
        btn.title = "hello"
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
       record_audio()
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }


}

