#include "uritests.h"
#include "../guiutil.h"
#include "../walletmodel.h"

#include <QUrl>

void URITests::uriTests()
{
    SendCoinsRecipient rv;
    QUrl uri;
    uri.setUrl(QString("icoin:BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j?req-dontexist="));
    QVERIFY(!GUIUtil::parseiCoinURI(uri, &rv));

    uri.setUrl(QString("icoin:BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j?dontexist="));
    QVERIFY(GUIUtil::parseiCoinURI(uri, &rv));
    QVERIFY(rv.address == QString("BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 0);

    uri.setUrl(QString("icoin:BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j?label=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseiCoinURI(uri, &rv));
    QVERIFY(rv.address == QString("BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j"));
    QVERIFY(rv.label == QString("Wikipedia Example Address"));
    QVERIFY(rv.amount == 0);

    uri.setUrl(QString("icoin:BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j?amount=0.001"));
    QVERIFY(GUIUtil::parseiCoinURI(uri, &rv));
    QVERIFY(rv.address == QString("BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100000);

    uri.setUrl(QString("icoin:BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j?amount=1.001"));
    QVERIFY(GUIUtil::parseiCoinURI(uri, &rv));
    QVERIFY(rv.address == QString("BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100100000);

    uri.setUrl(QString("icoin:BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j?amount=100&label=Wikipedia Example"));
    QVERIFY(GUIUtil::parseiCoinURI(uri, &rv));
    QVERIFY(rv.address == QString("BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j"));
    QVERIFY(rv.amount == 10000000000LL);
    QVERIFY(rv.label == QString("Wikipedia Example"));

    uri.setUrl(QString("icoin:BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j?message=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseiCoinURI(uri, &rv));
    QVERIFY(rv.address == QString("BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j"));
    QVERIFY(rv.label == QString());

    QVERIFY(GUIUtil::parseiCoinURI("icoin:BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j?message=Wikipedia Example Address", &rv));
    QVERIFY(rv.address == QString("BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j"));
    QVERIFY(rv.label == QString());

    // We currently don't implement the message parameter (ok, yea, we break spec...)
    uri.setUrl(QString("icoin:BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j?req-message=Wikipedia Example Address"));
    QVERIFY(!GUIUtil::parseiCoinURI(uri, &rv));

    uri.setUrl(QString("icoin:BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j?amount=1,000&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseiCoinURI(uri, &rv));

    uri.setUrl(QString("icoin:BYn7XhJG7Eyq38CLx4a3z2ZQ7AVpAoSw8j?amount=1,000.0&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseiCoinURI(uri, &rv));
}
